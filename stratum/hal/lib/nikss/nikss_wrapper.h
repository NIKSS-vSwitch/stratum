#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_WRAPPER_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_WRAPPER_H_

#include <string>

#include "absl/synchronization/mutex.h"
#include "stratum/glue/status/status.h"
#include "stratum/hal/lib/nikss/nikss_interface.h"

namespace stratum {
namespace hal {
namespace nikss {

// The "NikssWrapper" is an implementation of NikssInterface which is used
// to talk to the Linux eBPF subsystem via the NIKSS APIs calls.
class NikssWrapper : public NikssInterface {
 public:
  // NikssInterface public methods.
  ::util::Status AddPort(int pipeline_id,
                         const std::string& port_name);
  ::util::Status DelPort(int pipeline_id,
                         const std::string& port_name);
  ::util::Status AddPipeline(int pipeline_id,
                         const std::string filepath) override;

  static NikssWrapper* CreateSingleton() LOCKS_EXCLUDED(init_lock_);

  // NikssWrapper is neither copyable nor movable.
  NikssWrapper(const NikssWrapper&) = delete;
  NikssWrapper& operator=(const NikssWrapper&) = delete;
  NikssWrapper(NikssWrapper&&) = delete;
  NikssWrapper& operator=(NikssWrapper&&) = delete;

 protected:
  // RW mutex lock for protecting the singleton instance initialization and
  // reading it back from other threads. Unlike other singleton classes, we
  // use RW lock as we need the pointer to class to be returned.
  static absl::Mutex init_lock_;

  // The singleton instance.
  static NikssWrapper* singleton_ GUARDED_BY(init_lock_);

 private:

  // Private constructor, use CreateSingleton and GetSingleton().
  NikssWrapper();
};

}  // namespace nikss
}  // namespace hal
}  // namespace stratum


#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_WRAPPER_H_