#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_CHASSIS_MANAGER_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_CHASSIS_MANAGER_H_

#include <memory>

#include "absl/synchronization/mutex.h"
#include "stratum/hal/lib/common/phal_interface.h"

namespace stratum {
namespace hal {
namespace nikss {

// Lock which protects chassis state across the entire switch.
extern absl::Mutex chassis_lock;

class NikssChassisManager {
 public:
  virtual ~NikssChassisManager();

  // Factory function for creating the instance of the class.
  static std::unique_ptr<NikssChassisManager> CreateInstance(
      PhalInterface* phal_interface);

  // NikssChassisManager is neither copyable nor movable.
  NikssChassisManager(const NikssChassisManager&) = delete;
  NikssChassisManager& operator=(const NikssChassisManager&) = delete;
  NikssChassisManager(NikssChassisManager&&) = delete;
  NikssChassisManager& operator=(NikssChassisManager&&) = delete;

 private:
  // Private constructor. Use CreateInstance() to create an instance of this
  // class.
  NikssChassisManager(PhalInterface* phal_interface);

  bool initialized_ GUARDED_BY(chassis_lock);

  // Pointer to a PhalInterface implementation.
  PhalInterface* phal_interface_;  // not owned by this class.

};

}  // namespace nikss
}  // namespace hal
}  // namespace stratum


#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_CHASSIS_MANAGER_H_
