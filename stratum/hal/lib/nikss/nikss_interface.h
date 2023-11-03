#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_INTERFACE_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_INTERFACE_H_

#include "stratum/glue/status/status.h"

namespace stratum {
namespace hal {
namespace nikss {

class NikssInterface {
 public:
  // Add and initialize a NIKSS pipeline. The pipeline will be loaded
  // into the Linux eBPF subsystem. Can be used to re-initialize an existing device.
  virtual ::util::Status AddPipeline(int pipeline_id,
                                     const std::string filepath) = 0;

  // Add a new port with the given parameters.
  virtual ::util::Status AddPort(int pipeline_id, const std::string& port_name) = 0;

 protected:
  // Default constructor. To be called by the Mock class instance only.
  NikssInterface() {}
};

}  // namespace nikss
}  // namespace hal
}  // namespace stratum

#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_INTERFACE_H_