#include "stratum/hal/lib/nikss/nikss_chassis_manager.h"

#include "absl/synchronization/mutex.h"
#include "stratum/glue/gtl/map_util.h"
#include <utility>

namespace stratum {
namespace hal {
namespace nikss {

ABSL_CONST_INIT absl::Mutex chassis_lock(absl::kConstInit);

NikssChassisManager::NikssChassisManager(
    PhalInterface* phal_interface, NikssInterface* nikss_interface)
    : initialized_(false),
      phal_interface_(phal_interface),
      nikss_interface_(ABSL_DIE_IF_NULL(nikss_interface)) {
}

NikssChassisManager::~NikssChassisManager() = default;

namespace {

::util::Status AddPortHelper(NikssInterface* nikss_interface_, uint64 node_id, const std::string& port_name) {
  LOG(INFO) << "Adding port '" << port_name << "' to node " << node_id;
  RETURN_IF_ERROR(nikss_interface_->AddPort(node_id, port_name));
  return ::util::OkStatus();
}

}

std::unique_ptr<NikssChassisManager> NikssChassisManager::CreateInstance(
    PhalInterface* phal_interface, NikssInterface* nikss_interface) {
  return absl::WrapUnique(
      new NikssChassisManager(phal_interface, nikss_interface));
}

::util::Status NikssChassisManager::PushChassisConfig(
    const ChassisConfig& config) {
  ::util::Status status = ::util::OkStatus();  // errors to keep track of.
  
  std::map<uint64, std::map<uint32, PortConfig>> chassis_config;

  for (const auto& singleton_port : config.singleton_ports()) {
    PortConfig port = {
      .port_id = singleton_port.id(),
      .name = singleton_port.name(),
      .admin_state = singleton_port.config_params().admin_state(),
    };

    auto node_id = singleton_port.node();
    chassis_config[node_id][port.port_id] = port;
  }

/*
  // Compare ports in old config and new config and perform the necessary operations.
  for (auto& node : config.nodes()) {
    LOG(INFO) << "Updating config for node " << node.id() << ".";
    for (const auto& port_old : node_id_to_port_id_to_port_config_[node.id()]) {
      auto port_id = port_old.first;
      auto* singleton_port = gtl::FindOrNull(
          node_id_to_port_id_to_port_config[node.id()], port_id);

      if (singleton_port == nullptr) {  // remove port if not present anymore
        auto& config_old = port_old.second.config_params();
        if (config_old.admin_state() == ADMIN_STATE_ENABLED) {
          APPEND_STATUS_IF_ERROR(status, RemovePort(dev_mgr, node.id(), port_id));
        }
      } else {
        auto& config_old = port_old.second.config_params();
        auto& config = singleton_port->config_params();
        if (config.admin_state() != config_old.admin_state()) {
          if (config.admin_state() == ADMIN_STATE_ENABLED) {
//            APPEND_STATUS_IF_ERROR(
//                status,
//                AddPort(dev_mgr, node.id(), singleton_port->name(), port_id));
          } else {
//            APPEND_STATUS_IF_ERROR(status,
//                                   RemovePort(dev_mgr, node.id(), port_id));
//            if (node_id_to_port_id_to_port_state_[node.id()][port_id] ==
//                PORT_STATE_UP) {
//              // TODO(antonin): would it be better to just register a bmv2
//              // callback for PORT_REMOVED event?
//              VLOG(1) << "Sending DOWN notification for port " << port_id
//                      << " in node " << node.id() << ".";
//              SendPortOperStateGnmiEvent(node.id(), port_id, PORT_STATE_DOWN);
//            }
          }
        }
      }
    }


  }
  */

  chassis_config_ = chassis_config;
  
  return status;
}

::util::StatusOr<std::map<uint64, std::map<uint32, NikssChassisManager::PortConfig>>> NikssChassisManager::GetPortConfig() const {
  return chassis_config_;
}

}  // namespace nikss
}  // namespace hal
}  // namespace stratum