#include "stratum/hal/lib/nikss/nikss_switch.h"

#include "absl/memory/memory.h"
#include "absl/synchronization/mutex.h"
#include "stratum/hal/lib/nikss/nikss_node.h"

#include <nikss/nikss.h>

namespace stratum {
namespace hal {
namespace nikss {

NikssSwitch::NikssSwitch(PhalInterface* phal_interface,
                         NikssChassisManager* nikss_chassis_manager,
                         const absl::flat_hash_map<uint64, NikssNode*>& node_id_to_nikss_node)
    : phal_interface_(ABSL_DIE_IF_NULL(phal_interface)),
      nikss_chassis_manager_(ABSL_DIE_IF_NULL(nikss_chassis_manager)),
      node_id_to_nikss_node_(node_id_to_nikss_node) {
  for (const auto& entry : node_id_to_nikss_node_) {
    CHECK_NE(entry.second, nullptr)
        << "Detected null NikssNode for node_id " << entry.first << ".";
  }
}

NikssSwitch::~NikssSwitch() {}

::util::Status NikssSwitch::PushChassisConfig(const ChassisConfig& config) {
  LOG(INFO) << "Pushing chassis config";
  RETURN_IF_ERROR(nikss_chassis_manager_->PushChassisConfig(config));
  return ::util::OkStatus();
}

/*
::util::Status NikssSwitch::GetConfig(uint32 port_id) {
  LOG(INFO) << "Retrieving config from port " << port_id << ".";
  auto config = nikss_chassis_manager_->GetPortConfig(port_id);
  return ::util::OkStatus();
}
*/

::util::Status NikssSwitch::VerifyChassisConfig(const ChassisConfig& config) {
  LOG(INFO) << "VerifyChassisConfig";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::PushForwardingPipelineConfig(
    uint64 node_id, const ::p4::v1::ForwardingPipelineConfig& config) {
  
  LOG(INFO) << "Pushing P4-based forwarding pipeline to NIKSS";

  ASSIGN_OR_RETURN(auto* node, GetNikssNodeFromNodeId(node_id));
  ASSIGN_OR_RETURN(auto chassis_config, nikss_chassis_manager_->GetPortConfig());
  
  RETURN_IF_ERROR(node->PushForwardingPipelineConfig(config, chassis_config));

  LOG(INFO) << "P4-based forwarding pipeline config pushed successfully to "
            << "node with ID " << node_id << ".";
  
  return ::util::OkStatus();
}

::util::Status NikssSwitch::SaveForwardingPipelineConfig(
    uint64 node_id, const ::p4::v1::ForwardingPipelineConfig& config) {
  LOG(INFO) << "SaveForwardingPipelineConfig";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::CommitForwardingPipelineConfig(uint64 node_id) {
  LOG(INFO) << "CommitForwardingPipelineConfig";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::VerifyForwardingPipelineConfig(
    uint64 node_id, const ::p4::v1::ForwardingPipelineConfig& config) {
  LOG(INFO) << "VerifyForwardingPipelineConfig";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::Shutdown() {
  LOG(INFO) << "Shutdown";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::Freeze() { 
  return ::util::OkStatus(); 
}

::util::Status NikssSwitch::Unfreeze() { 
  return ::util::OkStatus(); 
}

::util::Status NikssSwitch::WriteForwardingEntries(
    const ::p4::v1::WriteRequest& req, std::vector<::util::Status>* results) {
  LOG(INFO) << "WriteForwardingEntries";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::ReadForwardingEntries(
    const ::p4::v1::ReadRequest& req,
    WriterInterface<::p4::v1::ReadResponse>* writer,
    std::vector<::util::Status>* details) {
  LOG(INFO) << "ReadForwardingEntries";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::RegisterStreamMessageResponseWriter(
    uint64 node_id,
    std::shared_ptr<WriterInterface<::p4::v1::StreamMessageResponse>> writer) {
  LOG(INFO) << "RegisterStreamMessageResponseWriter";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::UnregisterStreamMessageResponseWriter(
    uint64 node_id) {
  LOG(INFO) << "UnregisterStreamMessageResponseWriter";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::HandleStreamMessageRequest(
    uint64 node_id, const ::p4::v1::StreamMessageRequest& request) {
  LOG(INFO) << "HandleStreamMessageRequest";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::RegisterEventNotifyWriter(
    std::shared_ptr<WriterInterface<GnmiEventPtr>> writer) {
  LOG(INFO) << "RegisterEventNotifyWriter";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::UnregisterEventNotifyWriter() {
  LOG(INFO) << "UnregisterEventNotifyWriter";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::RetrieveValue(uint64 /*node_id*/,
                                         const DataRequest& request,
                                         WriterInterface<DataResponse>* writer,
                                         std::vector<::util::Status>* details) {
  LOG(INFO) << "RetrieveValue";
  return ::util::OkStatus();
}

::util::Status NikssSwitch::SetValue(uint64 node_id, const SetRequest& request,
                                    std::vector<::util::Status>* details) {
  LOG(INFO) << "SetValue";
  return ::util::OkStatus();
}

::util::StatusOr<std::vector<std::string>> NikssSwitch::VerifyState() {
  LOG(INFO) << "VerifyState";
  return std::vector<std::string>();
}

std::unique_ptr<NikssSwitch> NikssSwitch::CreateInstance(
    PhalInterface* phal_interface, NikssChassisManager* nikss_chassis_manager,
    const absl::flat_hash_map<uint64, NikssNode*>& node_id_to_nikss_node) {
  LOG(INFO) << "CreateInstance";
  return absl::WrapUnique(
      new NikssSwitch(phal_interface, nikss_chassis_manager, node_id_to_nikss_node));
}

::util::StatusOr<NikssNode*> NikssSwitch::GetNikssNodeFromNodeId(
    uint64 node_id) const {
  LOG(INFO) << "GetNikssNodeFromNodeId";
  NikssNode* node = gtl::FindPtrOrNull(node_id_to_nikss_node_, node_id);
  if (node == nullptr) {
    return MAKE_ERROR(ERR_ENTRY_NOT_FOUND)
           << "Node/Device " << node_id << " is unknown.";
  }
  return node;
}

}  // namespace bmv2
}  // namespace hal
}  // namespace nikss