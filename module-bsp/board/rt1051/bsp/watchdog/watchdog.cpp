#include "bsp/watchdog/watchdog.hpp"
extern "C"
{
#include "../../common/fsl_drivers/fsl_wdog.h"
}
#include <log/log.hpp>

#define WDOG_1_BASE          WDOG1
#define DEMO_WDOG_IRQHandler RTWDOG

namespace bsp::watchdog
{
    void init()
    {
        wdog_config_t config;
        WDOG_GetDefaultConfig(&config);
        config.timeoutValue = 0xFF; /* Timeout value is (0xFF + 1)/2 = 125 sec. */
        WDOG_Init(WDOG_1_BASE, &config);
        LOG_INFO("wdog init done-");
    }
    void system_reset()
    {
        LOG_DEBUG("request system reset to watchdog");
        WDOG_TriggerSystemSoftwareReset(WDOG_1_BASE);
    }

    void pet()
    {
        WDOG_Refresh(WDOG_1_BASE);
    }

    // taken straight from example wdog.c in NXP mcuxpresso
    std::string reset_cause()
    {
        auto resetFlag = WDOG_GetStatusFlags(WDOG_1_BASE);
        switch (resetFlag & (kWDOG_PowerOnResetFlag | kWDOG_TimeoutResetFlag | kWDOG_SoftwareResetFlag)) {
        case kWDOG_PowerOnResetFlag:
            return ("Power On Reset");
        case kWDOG_TimeoutResetFlag:
            return ("Time Out Reset!");
        case kWDOG_SoftwareResetFlag:
            return ("Software Reset!");
        default:
            return ("Error status!");
        }
    }
} // namespace bsp::watchdog
