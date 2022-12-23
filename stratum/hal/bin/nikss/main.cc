#include "gflags/gflags.h"
#include "stratum/glue/init_google.h"
#include "stratum/glue/logging.h"
#include "stratum/hal/lib/common/hal.h"

//extern "C" {
//#include "nikss/nikss.h"
//}

namespace stratum {
namespace hal {
namespace nikss {

::util::Status Main(int argc, char* argv[]) {
  InitGoogle(argv[0], &argc, &argv, true);
  InitStratumLogging();

//  nikss_context_t ctx;
//  nikss_context_init(&ctx);
//
//  nikss_context_free(&ctx);

  LOG(INFO) << "See you later!";
  return ::util::OkStatus();
}

}  // namespace barefoot
}  // namespace hal
}  // namespace nikss

int main(int argc, char* argv[]) {
  return stratum::hal::nikss::Main(argc, argv).error_code();
}