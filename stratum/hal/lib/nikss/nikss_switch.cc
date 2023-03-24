#include "stratum/hal/lib/nikss/nikss_switch.h"

#include "absl/memory/memory.h"
#include "absl/synchronization/mutex.h"

extern "C" {
#include "nikss/nikss.h"
}

namespace stratum {
namespace hal {
namespace nikss {

NikssSwitch::NikssSwitch(PhalInterface* phal_interface,
                         NikssChassisManager* nikss_chassis_manager)
    : phal_interface_(ABSL_DIE_IF_NULL(phal_interface)),
      nikss_chassis_manager_(ABSL_DIE_IF_NULL(nikss_chassis_manager)) {}

NikssSwitch::~NikssSwitch() {}

::util::Status NikssSwitch::PushChassisConfig(const ChassisConfig& config) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::VerifyChassisConfig(const ChassisConfig& config) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::PushForwardingPipelineConfig(
    uint64 node_id, const ::p4::v1::ForwardingPipelineConfig& config) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::SaveForwardingPipelineConfig(
    uint64 node_id, const ::p4::v1::ForwardingPipelineConfig& config) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::CommitForwardingPipelineConfig(uint64 node_id) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::VerifyForwardingPipelineConfig(
    uint64 node_id, const ::p4::v1::ForwardingPipelineConfig& config) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::Shutdown() {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::Freeze() { return ::util::OkStatus(); }

::util::Status NikssSwitch::Unfreeze() { return ::util::OkStatus(); }

::util::Status NikssSwitch::WriteForwardingEntries(
    const ::p4::v1::WriteRequest& req, std::vector<::util::Status>* results) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::ReadForwardingEntries(
    const ::p4::v1::ReadRequest& req,
    WriterInterface<::p4::v1::ReadResponse>* writer,
    std::vector<::util::Status>* details) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::RegisterStreamMessageResponseWriter(
    uint64 node_id,
    std::shared_ptr<WriterInterface<::p4::v1::StreamMessageResponse>> writer) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::UnregisterStreamMessageResponseWriter(
    uint64 node_id) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::HandleStreamMessageRequest(
    uint64 node_id, const ::p4::v1::StreamMessageRequest& request) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::RegisterEventNotifyWriter(
    std::shared_ptr<WriterInterface<GnmiEventPtr>> writer) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::UnregisterEventNotifyWriter() {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::RetrieveValue(uint64 /*node_id*/,
                                         const DataRequest& request,
                                         WriterInterface<DataResponse>* writer,
                                         std::vector<::util::Status>* details) {
  return ::util::OkStatus();
}

::util::Status NikssSwitch::SetValue(uint64 node_id, const SetRequest& request,
                                    std::vector<::util::Status>* details) {
  return ::util::OkStatus();
}

::util::StatusOr<std::vector<std::string>> NikssSwitch::VerifyState() {
  return std::vector<std::string>();
}

std::unique_ptr<NikssSwitch> NikssSwitch::CreateInstance(
    PhalInterface* phal_interface, NikssChassisManager* nikss_chassis_manager) {
  return absl::WrapUnique(
      new NikssSwitch(phal_interface, nikss_chassis_manager));
}

}  // namespace bmv2
}  // namespace hal
}  // namespace nikss
