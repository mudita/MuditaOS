#pragma once

#include "FreeRTOSConfig.h"

namespace sys
{

    enum class BusChannels
    {
        System,
        SystemManagerRequests,
        PowerManagerRequests
#ifndef MODULE_CORE_CUSTOM_BUS

#else
        ,
#include "BusChannelsCustom.hpp"
#endif
    };

    enum class ServicePriority
    {
        Idle     = 0, ///< priority: idle (lowest)
        Low      = 1, ///< priority: low
        Normal   = 2, ///< priority: normal
        High     = 3, ///< priority: high
        Realtime = 4, ///< priority: realtime (highest)
    };

    enum class ServicePowerMode
    {
        Active,
        SuspendToRAM,
        SuspendToNVM
    };

    enum class ReturnCodes
    {
        Success,
        Failure,
        Timeout,
        ServiceDoesntExist,
        // This is used in application's template in base class messages handler. The meaning is that
        // message that was processed by base class implementation of the DataReceivedHandler was not processed
        // and it should be handled by the class next in hierarchy.
        Unresolved
    };

} // namespace sys

inline const char *c_str(sys::ReturnCodes code)
{
    switch (code) {
    case sys::ReturnCodes::Success:
        return "Success";
    case sys::ReturnCodes::Failure:
        return "Failure";
    case sys::ReturnCodes::Timeout:
        return "Timeout";
    case sys::ReturnCodes::ServiceDoesntExist:
        return "ServiceDoesntExist";
    case sys::ReturnCodes::Unresolved:
        return "Unresolved";
    }
    return "Undefined";
}

inline const char *c_str(sys::ServicePowerMode code)
{
    switch (code) {
    case sys::ServicePowerMode::Active:
        return "Active";
    case sys::ServicePowerMode::SuspendToRAM:
        return "SuspendToRAM";
    case sys::ServicePowerMode::SuspendToNVM:
        return "SuspendToNVM";
    }
    return "";
}

inline const char *c_str(sys::BusChannels channel)
{
    switch (channel) {
    case sys::BusChannels::System:
        return "System";
    case sys::BusChannels::SystemManagerRequests:
        return "SystemManagerRequests";
    case sys::BusChannels::PowerManagerRequests:
        return "PowerManagerRequests";
    case sys::BusChannels::ServiceCellularNotifications:
        return "ServiceCellularNotifications,";
    case sys::BusChannels::Test2CustomBusChannel:
        return "Test2CustomBusChannel,";
    case sys::BusChannels::ServiceDBNotifications:
        return "ServiceDBNotifications,";
    case sys::BusChannels::ServiceAudioNotifications:
        return "ServiceAudioNotifications";
    case sys::BusChannels::AppManagerNotifications:
        return "AppManagerNotifications,";
    case sys::BusChannels::ServiceInternetNotifications:
        return "ServiceInternetNotifications";
    }
    return "";
}
