#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_WRAPPER_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_WRAPPER_H_

#include <string>

#include "absl/synchronization/mutex.h"
#include "absl/types/optional.h"
#include "stratum/glue/status/status.h"
#include "p4/v1/p4runtime.pb.h"
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
  ::util::Status TableContextInit(nikss_context_t* nikss_ctx,
      nikss_table_entry_t* entry,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_action_t* action_ctx,
      int node_id, std::string nikss_name);
  ::util::Status AddMatchesToEntry(const ::p4::v1::TableEntry& request,
      const ::p4::config::v1::Table table,
      nikss_table_entry_t* entry,
      bool type_insert_or_modify);
  ::util::Status AddActionsToEntry(const ::p4::v1::TableEntry& request,
      const ::p4::config::v1::Table table,
      const ::p4::config::v1::Action action,
      nikss_action_t* action_ctx,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_table_entry_t* entry);
  ::util::Status PushTableEntry(const ::p4::v1::Update::Type update_type,
      const ::p4::config::v1::Table table,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_table_entry_t* entry);
  ::util::StatusOr<::p4::v1::TableEntry> ReadTableEntry(
      const ::p4::v1::TableEntry& request,
      const ::p4::config::v1::Table table,
      nikss_table_entry_t* entry,
      nikss_table_entry_ctx_t* entry_ctx,
      std::map<std::string, NikssInterface::ActionData> table_actions);
  ::util::Status ReadSingleTable(
      const ::p4::v1::TableEntry& table_entry,
      const ::p4::config::v1::Table table,
      nikss_table_entry_t* entry,
      nikss_table_entry_ctx_t* entry_ctx,
      WriterInterface<::p4::v1::ReadResponse>* writer,
      std::map<std::string, NikssInterface::ActionData> table_actions,
      bool has_match_key);
  ::util::Status TableCleanup(nikss_context_t* nikss_ctx,
      nikss_table_entry_t* entry,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_action_t* action_ctx);
  ::util::Status CounterContextInit(nikss_context_t* nikss_ctx,
      nikss_counter_context_t* counter_ctx, 
      nikss_counter_entry_t* nikss_counter, int node_id, 
      std::string nikss_name);
  ::util::StatusOr<::p4::v1::CounterEntry> ReadCounterEntry(
      nikss_counter_entry_t* nikss_counter,
      nikss_counter_type_t counter_type);
  ::util::Status ReadSingleCounterEntry(
      const ::p4::v1::CounterEntry& counter_entry,
      nikss_counter_entry_t* nikss_counter,
      nikss_counter_context_t* counter_ctx,
      WriterInterface<::p4::v1::ReadResponse>* writer);
  ::util::Status ReadAllCounterEntries(
      const ::p4::v1::CounterEntry& counter_entry,
      nikss_counter_context_t* counter_ctx,
      WriterInterface<::p4::v1::ReadResponse>* writer);
  ::util::Status CounterCleanup(nikss_context_t* nikss_ctx,
      nikss_counter_context_t* counter_ctx,
      nikss_counter_entry_t* nikss_counter);

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

  // Auxiliary functions
  std::string ConvertToNikssName(std::string input_name);
  std::string SwapBytesOrder(std::string value);
  int ConvertBitwidthToSize(int bitwidth);

 private:

  // Private constructor, use CreateSingleton and GetSingleton().
  NikssWrapper();
};

}  // namespace nikss
}  // namespace hal
}  // namespace stratum


#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_WRAPPER_H_
