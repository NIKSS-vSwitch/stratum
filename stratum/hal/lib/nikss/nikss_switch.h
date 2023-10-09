#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_SWITCH_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_SWITCH_H_

#include "stratum/hal/lib/nikss/nikss_chassis_manager.h"
#include "stratum/hal/lib/nikss/nikss_node.h"
#include "stratum/hal/lib/common/switch_interface.h"

namespace stratum {
namespace hal {
namespace nikss {

class NikssSwitch : public SwitchInterface {
 public:
  ~NikssSwitch() override;

  // SwitchInterface public methods.
  ::util::Status PushChassisConfig(const ChassisConfig& config) override;
  ::util::Status VerifyChassisConfig(const ChassisConfig& config) override;
  ::util::Status PushForwardingPipelineConfig(
      uint64 node_id,
      const ::p4::v1::ForwardingPipelineConfig& config) override;
  ::util::Status SaveForwardingPipelineConfig(
      uint64 node_id,
      const ::p4::v1::ForwardingPipelineConfig& config) override;
  ::util::Status CommitForwardingPipelineConfig(uint64 node_id) override;
  ::util::Status VerifyForwardingPipelineConfig(
      uint64 node_id,
      const ::p4::v1::ForwardingPipelineConfig& config) override;
  ::util::Status Shutdown() override;
  ::util::Status Freeze() override;
  ::util::Status Unfreeze() override;
  ::util::Status WriteForwardingEntries(
      const ::p4::v1::WriteRequest& req,
      std::vector<::util::Status>* results) override;
  ::util::Status ReadForwardingEntries(
      const ::p4::v1::ReadRequest& req,
      WriterInterface<::p4::v1::ReadResponse>* writer,
      std::vector<::util::Status>* details) override;
  ::util::Status RegisterStreamMessageResponseWriter(
      uint64 node_id,
      std::shared_ptr<WriterInterface<::p4::v1::StreamMessageResponse>> writer)
      override;
  ::util::Status UnregisterStreamMessageResponseWriter(uint64 node_id) override;
  ::util::Status HandleStreamMessageRequest(
      uint64 node_id, const ::p4::v1::StreamMessageRequest& request) override;
  ::util::Status RegisterEventNotifyWriter(
      std::shared_ptr<WriterInterface<GnmiEventPtr>> writer) override;
  ::util::Status UnregisterEventNotifyWriter() override;
  ::util::Status RetrieveValue(uint64 node_id, const DataRequest& requests,
                               WriterInterface<DataResponse>* writer,
                               std::vector<::util::Status>* details) override
      LOCKS_EXCLUDED(chassis_lock);
  ::util::Status SetValue(uint64 node_id, const SetRequest& request,
                          std::vector<::util::Status>* details) override;
  ::util::StatusOr<std::vector<std::string>> VerifyState() override;


  static std::unique_ptr<NikssSwitch> CreateInstance(
      PhalInterface* phal_interface, NikssChassisManager* nikss_chassis_manager,
      const absl::flat_hash_map<uint64, NikssNode*>& node_id_to_nikss_node);

  // NikssSwitch is neither copyable nor movable.
  NikssSwitch(const NikssSwitch&) = delete;
  NikssSwitch& operator=(const NikssSwitch&) = delete;
  NikssSwitch(NikssSwitch&&) = delete;
  NikssSwitch& operator=(NikssSwitch&&) = delete;

 private:
  // Private constructor. Use CreateInstance() to create an instance of this
  // class.
  NikssSwitch(PhalInterface* phal_interface,
              NikssChassisManager* nikss_chassis_manager,
              const absl::flat_hash_map<uint64, NikssNode*>& node_id_to_nikss_node);

  // Helper to get NikssNode pointer from node_id number or return error
  // indicating invalid node_id.
  ::util::StatusOr<NikssNode*> GetNikssNodeFromNodeId(uint64 node_id) const;

  PhalInterface* phal_interface_;  // not owned by this class.

  // Per chassis Managers. Note that there is only one instance of this class
  // per chassis.
  NikssChassisManager* nikss_chassis_manager_;  // not owned by the class.

  // Map from zero-based node_id number corresponding to a node/pipeline to a
  // pointer to NikssNode.
  // This map is initialized in the constructor and will not change
  // during the lifetime of the class.
  // Pointers not owned.
  const absl::flat_hash_map<uint64, NikssNode*> node_id_to_nikss_node_;

};


}  // namespace nikss
}  // namespace hal
}  // namespace stratum

#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_SWITCH_H_
