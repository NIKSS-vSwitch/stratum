#include "stratum/hal/lib/nikss/nikss_node.h"

#include <memory>

#include "absl/synchronization/mutex.h"
#include "absl/memory/memory.h"
#include "stratum/hal/lib/common/proto_oneof_writer_wrapper.h"
#include "stratum/hal/lib/common/writer_interface.h"
#include "stratum/lib/macros.h"
#include "nikss/nikss.h"
#include "nikss/nikss_pipeline.h"

namespace stratum {
namespace hal {
namespace nikss {

NikssNode::NikssNode(NikssInterface* nikss_interface, 
                     uint64 node_id)
    : config_(),
      nikss_interface_(ABSL_DIE_IF_NULL(nikss_interface)),
      p4_info_manager_(nullptr),
      node_id_(node_id) {}

NikssNode::NikssNode()
    : config_(),
      nikss_interface_(nullptr),
      p4_info_manager_(nullptr),
      node_id_(0) {}

NikssNode::~NikssNode() = default;

// Factory function for creating the instance of the class.
std::unique_ptr<NikssNode> NikssNode::CreateInstance(
    NikssInterface* nikss_interface, 
    uint64 node_id) {
  return absl::WrapUnique(
      new NikssNode(nikss_interface, node_id));
}

::util::Status NikssNode::PushForwardingPipelineConfig(
    const ::p4::v1::ForwardingPipelineConfig& config,
    std::map<uint64, std::map<uint32, NikssChassisManager::PortConfig>> chassis_config) {
  // SaveForwardingPipelineConfig + CommitForwardingPipelineConfig
  RETURN_IF_ERROR(SaveForwardingPipelineConfig(config));

  auto p4_info = config.p4info();
  std::unique_ptr<P4InfoManager> p4_info_manager =
      absl::make_unique<P4InfoManager>(p4_info);
  RETURN_IF_ERROR(p4_info_manager->InitializeAndVerify());
  p4_info_manager_ = std::move(p4_info_manager);

  return CommitForwardingPipelineConfig(chassis_config);
}

::util::Status NikssNode::SaveForwardingPipelineConfig(
    const ::p4::v1::ForwardingPipelineConfig& config) {
  config_ = config;
  return ::util::OkStatus();
}

::util::Status NikssNode::CommitForwardingPipelineConfig(std::map<uint64, std::map<uint32, 
    NikssChassisManager::PortConfig>> chassis_config) {
  RETURN_IF_ERROR(nikss_interface_->AddPipeline(node_id_, config_.p4_device_config()));
  
  for (auto it = chassis_config[node_id_].begin(); it != chassis_config[node_id_].end(); it++) {
    uint32 key = it->first;
    NikssChassisManager::PortConfig config = it->second;
    RETURN_IF_ERROR(nikss_interface_->AddPort(node_id_, config.name));
  }
  return ::util::OkStatus();
}

::util::Status NikssNode::VerifyForwardingPipelineConfig(
    const ::p4::v1::ForwardingPipelineConfig& config) const {
  RET_CHECK(config.has_p4info()) << "Missing P4 info";
  RET_CHECK(!config.p4_device_config().empty()) << "Missing P4 device config";
  return ::util::OkStatus();
}

::util::Status NikssNode::WriteForwardingEntries(
    const ::p4::v1::WriteRequest& req, std::vector<::util::Status>* results) {
  absl::WriterMutexLock l(&lock_);
  RET_CHECK(req.device_id() == node_id_)
      << "Request device id must be same as id of this NikssNode.";
  RET_CHECK(req.atomicity() == ::p4::v1::WriteRequest::CONTINUE_ON_ERROR)
      << "Request atomicity "
      << ::p4::v1::WriteRequest::Atomicity_Name(req.atomicity())
      << " is not supported.";

  bool success = true;
  for (const auto& update : req.updates()) {
    ::util::Status status = ::util::OkStatus();
    switch (update.entity().entity_case()) {
      case ::p4::v1::Entity::kTableEntry: {
        status = WriteTableEntry(
            update.type(), update.entity().table_entry());
        break;
      }
      default: {
        status = MAKE_ERROR(ERR_UNIMPLEMENTED)
                 << "Unsupported entity type: " << update.ShortDebugString();
        break;
      }
    }
    success &= status.ok();
    results->push_back(status);
  }

  if (!success) {
    return MAKE_ERROR(ERR_AT_LEAST_ONE_OPER_FAILED)
           << "One or more write operations failed.";
  }

  LOG(INFO) << "P4-based forwarding entities written successfully to node with "
            << "ID " << node_id_ << ".";
  return ::util::OkStatus();
}

::util::Status NikssNode::WriteTableEntry(
    const ::p4::v1::Update::Type update_type,
    const ::p4::v1::TableEntry& table_entry) {

  auto table_id = table_entry.table_id();
  auto action_id = table_entry.action().action().action_id();
  ASSIGN_OR_RETURN(auto table, p4_info_manager_->FindTableByID(
                                  table_id));
  ASSIGN_OR_RETURN(auto action, p4_info_manager_->FindActionByID(
                                  action_id));
  auto name = table.preamble().name();
  
  // Nikss contexts declaration
  auto nikss_ctx = absl::make_unique<nikss_context_t>();
  auto entry = absl::make_unique<nikss_table_entry_t>();
  auto entry_ctx = absl::make_unique<nikss_table_entry_ctx_t>();
  auto action_ctx = absl::make_unique<nikss_action_t>();

  bool insert_or_modify_entry = false;
  if (update_type == ::p4::v1::Update::INSERT || 
      update_type == ::p4::v1::Update::MODIFY){
    insert_or_modify_entry = true;
  }

  ::util::Status status;
  // Init nikss contexts
  status = nikss_interface_->TableContextInit(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                              action_ctx.get(), node_id_, name);
  if (status != ::util::OkStatus()){
    nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                   action_ctx.get());
    return status;
  }

  // Add matches from request to entry
  status = nikss_interface_->AddMatchesToEntry(table_entry, table, entry.get(),
                                               insert_or_modify_entry);
  if (status != ::util::OkStatus()){
    nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                   action_ctx.get());
    return status;
  }

  // Add actions from request to entry only if necessary (insert or modify request)
  if (insert_or_modify_entry){
    status = nikss_interface_->AddActionsToEntry(table_entry, table, action,
                                     action_ctx.get(), entry_ctx.get(), entry.get());
    if (status != ::util::OkStatus()){
      nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                     action_ctx.get());
      return status;
    }
  }

  // Push table entry
  status = nikss_interface_->PushTableEntry(update_type, table, entry_ctx.get(), entry.get());
  if (status != ::util::OkStatus()){
    nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                   action_ctx.get());
    return status;
  }

  // Cleanup
  nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                 action_ctx.get());

  return ::util::OkStatus();
}

std::string NikssNode::ConvertToNikssName(std::string input_name){
  std::replace(input_name.begin(), input_name.end(), '.', '_');
  return input_name;
}

::util::Status NikssNode::ReadForwardingEntries(
    const ::p4::v1::ReadRequest& req,
    WriterInterface<::p4::v1::ReadResponse>* writer,
    std::vector<::util::Status>* details) {
  RET_CHECK(writer) << "Channel writer must be non-null.";
  RET_CHECK(details) << "Details pointer must be non-null.";

  absl::ReaderMutexLock l(&lock_);
  RET_CHECK(req.device_id() == node_id_)
      << "Request device id must be same as id of this NIKSS.";
  bool success = true;
  for (const auto& entity : req.entities()) {
    switch (entity.entity_case()) {
      case ::p4::v1::Entity::kTableEntry: {
        auto status = ReadTableEntry(
            entity.table_entry(), writer);
        success &= status.ok();
        details->push_back(status);
        break;
      }
      case ::p4::v1::Entity::kCounterEntry: {
        auto status = ReadIndirectCounterEntry(
            entity.counter_entry(), writer);
        success &= status.ok();
        details->push_back(status);
        break;
      }
      default: {
        success = false;
        details->push_back(MAKE_ERROR(ERR_UNIMPLEMENTED)
                           << "Unsupported entity type: "
                           << entity.ShortDebugString());
        break;
      }
    }
  }
  if (!success) {
    return MAKE_ERROR(ERR_AT_LEAST_ONE_OPER_FAILED)
           << "One or more read operations failed.";
  }
  return ::util::OkStatus();
}

::util::Status NikssNode::ReadTableEntry(
    const ::p4::v1::TableEntry& table_entry,
    WriterInterface<::p4::v1::ReadResponse>* writer) {

  auto table_id = table_entry.table_id();
  if (table_id == 0){
    return MAKE_ERROR(ERR_OPER_NOT_SUPPORTED)
            << "Querying a table without table id is not supported.";
  }
  
  auto nikss_ctx = absl::make_unique<nikss_context_t>();
  auto entry = absl::make_unique<nikss_table_entry_t>();
  auto entry_ctx = absl::make_unique<nikss_table_entry_ctx_t>();
  auto action_ctx = absl::make_unique<nikss_action_t>();
  ::util::Status status;

  ASSIGN_OR_RETURN(auto table, p4_info_manager_->FindTableByID(
                                  table_id));
  auto name = table.preamble().name();

  std::map<std::string, NikssInterface::ActionData> table_actions;
  for (const auto& p4info_action : table.action_refs()){
    uint32 id = p4info_action.id();
    ASSIGN_OR_RETURN(auto action, p4_info_manager_->FindActionByID(id));
    std::string name = ConvertToNikssName(action.preamble().name());
    std::vector<int32> bitwidths;
    for (auto bitwidth : action.params()){
      bitwidths.push_back(bitwidth.bitwidth());
    }
    NikssInterface::ActionData data = {
      .action_id = id,
      .bitwidths = bitwidths,
    };
    table_actions[name] = data;
  }

  // Init nikss contexts
  status = nikss_interface_->TableContextInit(nikss_ctx.get(), entry.get(), 
                                              entry_ctx.get(), action_ctx.get(), 
                                              node_id_, name);
  if (status != ::util::OkStatus()){
    nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                   action_ctx.get());
    return status;
  }

  // Check if match key is provided
  bool has_match_key = table_entry.match_size() != 0;

  // Add matches from request to entry if match key is provided
  if (has_match_key){
    status = nikss_interface_->AddMatchesToEntry(table_entry, table, entry.get(), false);
    if (status != ::util::OkStatus()){
      nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                     action_ctx.get());
      return status;
    }
  }

  status = nikss_interface_->ReadSingleTable(table_entry, table,
                                             entry.get(), entry_ctx.get(), 
                                             writer,
                                             table_actions, has_match_key);
  if (status != ::util::OkStatus()){
    nikss_interface_->TableCleanup(nikss_ctx.get(), entry.get(), entry_ctx.get(), 
                                   action_ctx.get());
    return status;
  }

  return ::util::OkStatus();
}

::util::Status NikssNode::ReadIndirectCounterEntry(
    const ::p4::v1::CounterEntry& counter_entry,
    WriterInterface<::p4::v1::ReadResponse>* writer) {

  RET_CHECK(counter_entry.counter_id() != 0)
    << "Querying an indirect counter without counter id is not supported.";
  RET_CHECK(counter_entry.index().index() >= 0)
    << "Counter index must be greater than or equal to zero.";

  auto counter_id = counter_entry.counter_id();
  ASSIGN_OR_RETURN(auto counter, p4_info_manager_->FindCounterByID(
                                  counter_id));
  auto name = counter.preamble().name();

  auto nikss_ctx = absl::make_unique<nikss_context_t>();
  auto counter_ctx = absl::make_unique<nikss_counter_context_t>();
  auto nikss_counter = absl::make_unique<nikss_counter_entry_t>();

  ::util::Status status;
  // Init nikss contexts
  status = nikss_interface_->CounterContextInit(nikss_ctx.get(), counter_ctx.get(), 
                                                nikss_counter.get(), node_id_, name);
  if (status != ::util::OkStatus()){
    nikss_interface_->CounterCleanup(nikss_ctx.get(), counter_ctx.get(), nikss_counter.get());
    return status;
  }

  // Reading counter entry with index provided
  if (counter_entry.has_index()) {
    status = nikss_interface_->ReadSingleCounterEntry(counter_entry, 
                                                      nikss_counter.get(), 
                                                      counter_ctx.get(), 
                                                      writer);
    if (status != ::util::OkStatus()){
      nikss_interface_->CounterCleanup(nikss_ctx.get(), counter_ctx.get(), nikss_counter.get());
      return status;
    }
    
  // Reading all counter entries
  } else {
    status = nikss_interface_->ReadAllCounterEntries(counter_entry, 
                                                     counter_ctx.get(), 
                                                     writer);
    if (status != ::util::OkStatus()){
      nikss_interface_->CounterCleanup(nikss_ctx.get(), counter_ctx.get(), nikss_counter.get());
      return status;
    }
  }

  // Cleanup
  nikss_interface_->CounterCleanup(nikss_ctx.get(), counter_ctx.get(), nikss_counter.get());

  return ::util::OkStatus();
}

}  // namespace nikss
}  // namespace hal
}  // namespace stratum
