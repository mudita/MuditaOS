// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "chip.hpp"

#include <log/log.hpp>

#include <map>
#include <string>

namespace
{
    const std::map<std::uint32_t, std::string> bootReasonDef = {
        {SRC_SRSR_IPP_RESET_B_SHIFT, "ipp_reset_b pin (Power-up sequence)"},
        {SRC_SRSR_LOCKUP_SYSRESETREQ_SHIFT, "CPU lockup or software setting of SYSRESETREQ"},
        {SRC_SRSR_CSU_RESET_B_SHIFT, "csu_reset_b input"},
        {SRC_SRSR_IPP_USER_RESET_B_SHIFT, "ipp_user_reset_b qualified reset"},
        {SRC_SRSR_WDOG_RST_B_SHIFT, "IC Watchdog Time-out reset"},
        {SRC_SRSR_JTAG_RST_B_SHIFT, "HIGH - Z JTAG reset"},
        {SRC_SRSR_JTAG_SW_RST_SHIFT, "JTAG software reset"},
        {SRC_SRSR_WDOG3_RST_B_SHIFT, "IC Watchdog3 Time-out reset"},
        {SRC_SRSR_TEMPSENSE_RST_B_SHIFT, "Tamper Sensor software reset"}};
} // namespace

void clearAndPrintBootReason()
{
    // get boot reason
    std::uint32_t SRSR_val = SRC_GetResetStatusFlags(SRC);

    LOG_INFO("Boot reason: %" PRIu32, SRSR_val);
    if (SRSR_val & (1UL << SRC_SRSR_IPP_RESET_B_SHIFT))
        LOG_INFO("\t*%s", bootReasonDef.at(SRC_SRSR_IPP_RESET_B_SHIFT).c_str());
    if (SRSR_val & (1UL << SRC_SRSR_LOCKUP_SYSRESETREQ_SHIFT)) {
        LOG_WARN("\t*%s", bootReasonDef.at(SRC_SRSR_LOCKUP_SYSRESETREQ_SHIFT).c_str());
        LOG_INFO("\tGPR_5 %" PRIu32, SRC_GetGeneralPurposeRegister(SRC, 5));
    }
    if (SRSR_val & (1UL << SRC_SRSR_CSU_RESET_B_SHIFT))
        LOG_INFO("\t*%s", bootReasonDef.at(SRC_SRSR_CSU_RESET_B_SHIFT).c_str());
    if (SRSR_val & (1UL << SRC_SRSR_IPP_USER_RESET_B_SHIFT))
        LOG_INFO("\t*%s", bootReasonDef.at(SRC_SRSR_IPP_USER_RESET_B_SHIFT).c_str());
    if (SRSR_val & (1UL << SRC_SRSR_WDOG_RST_B_SHIFT))
        LOG_WARN("\t*%s", bootReasonDef.at(SRC_SRSR_WDOG_RST_B_SHIFT).c_str());
    if (SRSR_val & (1UL << SRC_SRSR_JTAG_RST_B_SHIFT))
        LOG_INFO("\t*%s", bootReasonDef.at(SRC_SRSR_JTAG_RST_B_SHIFT).c_str());
    if (SRSR_val & (1UL << SRC_SRSR_JTAG_SW_RST_SHIFT))
        LOG_INFO("\t*%s", bootReasonDef.at(SRC_SRSR_JTAG_SW_RST_SHIFT).c_str());
    if (SRSR_val & (1UL << SRC_SRSR_WDOG3_RST_B_SHIFT))
        LOG_INFO("\t*%s", bootReasonDef.at(SRC_SRSR_WDOG3_RST_B_SHIFT).c_str());
    if (SRSR_val & (1UL << SRC_SRSR_TEMPSENSE_RST_B_SHIFT))
        LOG_WARN("\t*%s", bootReasonDef.at(SRC_SRSR_TEMPSENSE_RST_B_SHIFT).c_str());

    // clear all set bits
    SRC_ClearResetStatusFlags(SRC, SRSR_val);
}
