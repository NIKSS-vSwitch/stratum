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

  chassis_config_ = chassis_config;
  
  return status;
}

::util::StatusOr<std::map<uint64, std::map<uint32, NikssChassisManager::PortConfig>>> NikssChassisManager::GetPortConfig() const {
  return chassis_config_;
}

}  // namespace nikss
}  // namespace hal
}  // namespace stratum
