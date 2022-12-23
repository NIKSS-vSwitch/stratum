// Copyright 2018-present Barefoot Networks, Inc.
// SPDX-License-Identifier: Apache-2.0

#ifndef STRATUM_HAL_LIB_NIKSS_NIKSS_SWITCH_H_
#define STRATUM_HAL_LIB_NIKSS_NIKSS_SWITCH_H_

#include "stratum/hal/lib/common/switch_interface.h"

namespace stratum {
namespace hal {
namespace nikss {

class NikssSwitch : public SwitchInterface {
 public:
  ~NikssSwitch() override;

};


}  // namespace bmv2
}  // namespace hal
}  // namespace nikss

#endif  // STRATUM_HAL_LIB_NIKSS_NIKSS_SWITCH_H_


