#include "stratum/hal/lib/nikss/nikss_node.h"

#include <memory>

#include "absl/synchronization/mutex.h"
#include "absl/memory/memory.h"
#include "stratum/lib/macros.h"

namespace stratum {
namespace hal {
namespace nikss {

NikssNode::NikssNode(NikssInterface* nikss_interface, uint64 node_id)
    : config_(),
      nikss_interface_(ABSL_DIE_IF_NULL(nikss_interface)),
      //nikss_switch_(ABSL_DIE_IF_NULL(nikss_switch)),
      node_id_(node_id) {}

NikssNode::NikssNode()
    : nikss_interface_(nullptr),
      node_id_(0) {}

NikssNode::~NikssNode() = default;

// Factory function for creating the instance of the class.
std::unique_ptr<NikssNode> NikssNode::CreateInstance(
    NikssInterface* nikss_interface, uint64 node_id) {
  return absl::WrapUnique(
      new NikssNode(nikss_interface, node_id));
}

::util::Status NikssNode::PushForwardingPipelineConfig(
    const ::p4::v1::ForwardingPipelineConfig& config,
    std::map<uint64, std::map<uint32, NikssChassisManager::PortConfig>> chassis_config) {
  // SaveForwardingPipelineConfig + CommitForwardingPipelineConfig
  RETURN_IF_ERROR(SaveForwardingPipelineConfig(config));
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
    LOG(INFO) << "Adding new port with name " << config.name << ".";
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

}  // namespace nikss
}  // namespace hal
}  // namespace stratum