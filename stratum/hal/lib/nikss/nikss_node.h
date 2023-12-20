#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_NODE_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_NODE_H_

#include "absl/synchronization/mutex.h"
#include "p4/v1/p4runtime.grpc.pb.h"
#include "p4/v1/p4runtime.pb.h"
#include "stratum/glue/integral_types.h"
#include "stratum/glue/status/status.h"

#include "stratum/hal/lib/nikss/nikss_interface.h"
#include "stratum/hal/lib/nikss/nikss_chassis_manager.h"
#include "stratum/hal/lib/p4/p4_info_manager.h"
#include "stratum/hal/lib/common/writer_interface.h"

namespace stratum {
namespace hal {
namespace nikss {

class NikssNode {
 public:
  virtual ~NikssNode();

  virtual ::util::Status PushForwardingPipelineConfig(
      const ::p4::v1::ForwardingPipelineConfig& config,
      std::map<uint64, std::map<uint32, NikssChassisManager::PortConfig>> chassis_config);
  virtual ::util::Status SaveForwardingPipelineConfig(
      const ::p4::v1::ForwardingPipelineConfig& config) LOCKS_EXCLUDED(lock_);
  virtual ::util::Status CommitForwardingPipelineConfig(
  	   std::map<uint64, std::map<uint32, NikssChassisManager::PortConfig>> chassis_config) LOCKS_EXCLUDED(lock_);
  virtual ::util::Status VerifyForwardingPipelineConfig(
      const ::p4::v1::ForwardingPipelineConfig& config) const;
  virtual ::util::Status WriteForwardingEntries(
      const ::p4::v1::WriteRequest& req, std::vector<::util::Status>* results)
      LOCKS_EXCLUDED(lock_);
  virtual ::util::Status WriteTableEntry(
      const ::p4::v1::Update::Type type,
      const ::p4::v1::TableEntry& table_entry) LOCKS_EXCLUDED(lock_);
  virtual ::util::Status ReadForwardingEntries(
      const ::p4::v1::ReadRequest& req,
      WriterInterface<::p4::v1::ReadResponse>* writer,
      std::vector<::util::Status>* details) LOCKS_EXCLUDED(lock_);
  virtual ::util::Status ReadTableEntry(
      const ::p4::v1::TableEntry& table_entry,
      WriterInterface<::p4::v1::ReadResponse>* writer) LOCKS_EXCLUDED(lock_);
  virtual ::util::Status ReadIndirectCounterEntry(
      const ::p4::v1::CounterEntry& counter_entry,
      WriterInterface<::p4::v1::ReadResponse>* writer) LOCKS_EXCLUDED(lock_);

  // Factory function for creating the instance of the class.
  static std::unique_ptr<NikssNode> CreateInstance(
      NikssInterface* nikss_interface, 
      uint64 node_id);

  // NikssNode is neither copyable nor movable.
  NikssNode(const NikssNode&) = delete;
  NikssNode& operator=(const NikssNode&) = delete;
  NikssNode(NikssNode&&) = delete;
  NikssNode& operator=(NikssNode&&) = delete;

 protected:
  // Default constructor. To be called by the Mock class instance only.
  NikssNode();

  // Auxiliary functions
  std::string ConvertToNikssName(std::string input_name);

 private:
  // Private constructor. Use CreateInstance() to create an instance of this
  // class.
  NikssNode(NikssInterface* nikss_interface, 
    uint64 node_id);

  // Reader-writer lock used to protect access to node-specific state.
  mutable absl::Mutex lock_;

  // Stores pipeline information for this node.
  ::p4::v1::ForwardingPipelineConfig config_ GUARDED_BY(lock_);

  // Pointer to a NikssInterface implementation that wraps all the SDE calls.
  // Not owned by this class.
  NikssInterface* nikss_interface_ = nullptr;

  // Helper class to validate the P4Info and requests against it.
  std::unique_ptr<P4InfoManager> p4_info_manager_ GUARDED_BY(lock_);

  // Logical node ID corresponding to the node/pipeline managed by this class
  // instance.
  uint64 node_id_ GUARDED_BY(lock_);
};


}  // namespace nikss
}  // namespace hal
}  // namespace stratum

#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_NODE_H_
