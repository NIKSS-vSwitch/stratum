#include "stratum/hal/lib/nikss/nikss_chassis_manager.h"

namespace stratum {
namespace hal {
namespace nikss {

NikssChassisManager::NikssChassisManager(
    PhalInterface* phal_interface)
    : initialized_(false),
      phal_interface_(phal_interface) {
}

NikssChassisManager::~NikssChassisManager() = default;

std::unique_ptr<NikssChassisManager> NikssChassisManager::CreateInstance(
    PhalInterface* phal_interface) {
  return absl::WrapUnique(
      new NikssChassisManager(phal_interface));
}

}  // namespace nikss
}  // namespace hal
}  // namespace stratum
