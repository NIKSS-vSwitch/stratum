#include "gflags/gflags.h"
#include "stratum/glue/init_google.h"
#include "stratum/glue/logging.h"
#include "stratum/hal/lib/nikss/nikss_switch.h"
#include "stratum/hal/lib/common/hal.h"
#include "stratum/hal/lib/phal/phal_sim.h"
#include "stratum/lib/security/auth_policy_checker.h"
#include "stratum/lib/security/credentials_manager.h"

#include "nikss/nikss.h"

namespace stratum {
namespace hal {
namespace nikss {

::util::Status Main(int argc, char* argv[]) {
  InitGoogle(argv[0], &argc, &argv, true);
  InitStratumLogging();

  auto* phal_sim = PhalSim::CreateSingleton();
  auto nikss_chassis_manager =
      NikssChassisManager::CreateInstance(phal_sim);

  auto nikss_switch = NikssSwitch::CreateInstance(
      phal_sim, nikss_chassis_manager.get());

  // Create the 'Hal' class instance.
  auto auth_policy_checker = AuthPolicyChecker::CreateInstance();
  ASSIGN_OR_RETURN(auto credentials_manager,
                   CredentialsManager::CreateInstance());
  auto* hal = Hal::CreateSingleton(stratum::hal::OPERATION_MODE_SIM,
                                   nikss_switch.get(), auth_policy_checker.get(),
                                   credentials_manager.get());
  RET_CHECK(hal) << "Failed to create the Stratum Hal instance.";

  // Setup and start serving RPCs.
  ::util::Status status = hal->Setup();
  if (!status.ok()) {
    LOG(ERROR)
        << "Error when setting up Stratum HAL (but we will continue running): "
        << status.error_message();
  }

  RETURN_IF_ERROR(hal->Run());  // blocking
  LOG(INFO) << "See you later!";
  return ::util::OkStatus();
}

}  // namespace nikss
}  // namespace hal
}  // namespace stratum

int main(int argc, char* argv[]) {
  return stratum::hal::nikss::Main(argc, argv).error_code();
}
