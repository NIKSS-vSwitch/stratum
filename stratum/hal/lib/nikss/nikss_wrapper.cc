#include "stratum/hal/lib/nikss/nikss_wrapper.h"

#include <memory>
#include <set>
#include <utility>
#include <fstream>
#include <string>
#include <iostream>

#include "absl/memory/memory.h"
#include "absl/synchronization/mutex.h"
#include "stratum/glue/status/status.h"
#include "stratum/lib/utils.h"
#include "stratum/lib/macros.h"

extern "C" {
#include "nikss/nikss.h"
#include "nikss/nikss_pipeline.h"
}

namespace stratum {
namespace hal {
namespace nikss {

NikssWrapper* NikssWrapper::singleton_ = nullptr;
ABSL_CONST_INIT absl::Mutex NikssWrapper::init_lock_(absl::kConstInit);

NikssWrapper::NikssWrapper() {}

::util::Status NikssWrapper::AddPipeline(int pipeline_id,
                                         const std::string bpf_obj) {
  // FIXME: nikss currently doesn't support loading BPF programs from memory.
  //  So, we save it to the disk first and let NIKSS load it from the disk.
  RETURN_IF_ERROR(WriteStringToFile(bpf_obj, "/tmp/stratum/bpf.o"));


  auto ctx = absl::make_unique<nikss_context_t>();
  nikss_context_init(ctx.get());
  nikss_context_set_pipeline(ctx.get(), static_cast<nikss_pipeline_id_t>(pipeline_id));

  if (nikss_pipeline_exists(ctx.get())) {
    LOG(INFO) << "NIKSS pipeline already exists, re-pushing is not supported yet.";
    return ::util::OkStatus();
  }

  // TODO: use RETURN_IF_NIKSS_ERROR macro
  nikss_pipeline_load(ctx.get(), "/tmp/stratum/bpf.o");

  RemoveFile("/tmp/stratum/bpf.o");

  return ::util::OkStatus();
}

NikssWrapper* NikssWrapper::CreateSingleton() {
  absl::WriterMutexLock l(&init_lock_);
  if (!singleton_) {
    singleton_ = new NikssWrapper();
  }

  return singleton_;
}

}  // namespace nikss
}  // namespace hal
}  // namespace stratum
