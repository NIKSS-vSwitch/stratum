#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_INTERFACE_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_INTERFACE_H_

#include "stratum/glue/status/status.h"
#include "stratum/glue/status/statusor.h"
#include "stratum/glue/integral_types.h"
#include "stratum/hal/lib/common/writer_interface.h"
#include "p4/v1/p4runtime.pb.h"
#include "nikss/nikss.h"

namespace stratum {
namespace hal {
namespace nikss {

class NikssInterface {
 public:
  
  struct ActionData {
    uint32 action_id;
    std::vector<int32> bitwidths;
  };

  // Add a new port with the given parameters.
  virtual ::util::Status AddPort(int pipeline_id,
      const std::string& port_name) = 0;

  // Add and initialize a NIKSS pipeline. The pipeline will be loaded
  // into the Linux eBPF subsystem. Can be used to re-initialize an existing device.
  virtual ::util::Status AddPipeline(int pipeline_id,
      const std::string filepath) = 0;

  // Init Nikss Table Contexts
  virtual ::util::Status TableContextInit(nikss_context_t* nikss_ctx,
      nikss_table_entry_t* entry,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_action_t* action_ctx,
      int node_id, std::string nikss_name) = 0;

  // Add matches from request to entry
  virtual ::util::Status AddMatchesToEntry(const ::p4::v1::TableEntry& request,
      const ::p4::config::v1::Table table,
      nikss_table_entry_t* entry,
      bool type_insert_or_modify) = 0;

  // Add actions from request to entry
  virtual ::util::Status AddActionsToEntry(const ::p4::v1::TableEntry& request,
      const ::p4::config::v1::Table table,
      const ::p4::config::v1::Action action,
      nikss_action_t* action_ctx,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_table_entry_t* entry) = 0;

  // Push table entry
  virtual ::util::Status PushTableEntry(const ::p4::v1::Update::Type update_type,
      const ::p4::config::v1::Table table,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_table_entry_t* entry) = 0;

  // Read specified table
  virtual ::util::Status ReadSingleTable(
      const ::p4::v1::TableEntry& table_entry,
      const ::p4::config::v1::Table table,
      nikss_table_entry_t* entry,
      nikss_table_entry_ctx_t* entry_ctx,
      WriterInterface<::p4::v1::ReadResponse>* writer,
      std::map<std::string, ActionData> table_actions,
      bool has_match_key) = 0;

  // Cleanup Table
  virtual ::util::Status TableCleanup(nikss_context_t* nikss_ctx,
      nikss_table_entry_t* entry,
      nikss_table_entry_ctx_t* entry_ctx,
      nikss_action_t* action_ctx) = 0;

  // Init Nikss Counter Contexts
  virtual ::util::Status CounterContextInit(nikss_context_t* nikss_ctx,
      nikss_counter_context_t* counter_ctx,
      nikss_counter_entry_t* nikss_counter,
      int node_id, std::string nikss_name) = 0;

  // Read counter with specified index
  virtual ::util::Status ReadSingleCounterEntry(
      const ::p4::v1::CounterEntry& counter_entry,
      nikss_counter_entry_t* nikss_counter,
      nikss_counter_context_t* counter_ctx,
      WriterInterface<::p4::v1::ReadResponse>* writer) = 0;

  // Read all counters
  virtual ::util::Status ReadAllCounterEntries(
      const ::p4::v1::CounterEntry& counter_entry,
      nikss_counter_context_t* counter_ctx,
      WriterInterface<::p4::v1::ReadResponse>* writer) = 0;

  // Cleanup Counter
  virtual ::util::Status CounterCleanup(nikss_context_t* nikss_ctx,
      nikss_counter_context_t* counter_ctx,
      nikss_counter_entry_t* nikss_counter) = 0;

 protected:
  // Default constructor. To be called by the Mock class instance only.
  NikssInterface() {}
};

}  // namespace nikss
}  // namespace hal
}  // namespace stratum

#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_INTERFACE_H_
