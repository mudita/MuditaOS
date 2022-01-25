// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <system/SystemReturnCodes.hpp>

namespace sys
{

    enum class BusChannel
    {
        Unknown,
        System,
        SystemManagerRequests,
        PowerManagerRequests,
        ServiceCellularNotifications,
        Test2CustomBusChannel,
        ServiceDBNotifications,
        ServiceAudioNotifications,
        AppManagerNotifications,
        ServiceFotaNotifications,
        AntennaNotifications,
        ServiceEvtmgrNotifications,
        PhoneModeChanges,
        PhoneLockChanges,
        BluetoothModeChanges,
        BluetoothNotifications
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

    enum class CloseReason
    {
        RegularPowerDown,
        Reboot,
        RebootToUpdate,
        RebootToUsbMscMode,
        FactoryReset,
        SystemBrownout,
        LowBattery
    };

    // Updater reason code
    enum class UpdateReason
    {
        Update,
        Recovery,
        FactoryReset
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

inline const char *c_str(sys::BusChannel channel)
{
    switch (channel) {
    case sys::BusChannel::Unknown:
        return "Unknown";
    case sys::BusChannel::System:
        return "System";
    case sys::BusChannel::SystemManagerRequests:
        return "SystemManagerRequests";
    case sys::BusChannel::PowerManagerRequests:
        return "PowerManagerRequests";
    case sys::BusChannel::ServiceCellularNotifications:
        return "ServiceCellularNotifications,";
    case sys::BusChannel::Test2CustomBusChannel:
        return "Test2CustomBusChannel,";
    case sys::BusChannel::ServiceDBNotifications:
        return "ServiceDBNotifications,";
    case sys::BusChannel::ServiceAudioNotifications:
        return "ServiceAudioNotifications";
    case sys::BusChannel::AppManagerNotifications:
        return "AppManagerNotifications,";
    case sys::BusChannel::ServiceFotaNotifications:
        return "ServiceFotaNotifications";
    case sys::BusChannel::AntennaNotifications:
        return "AntennaNotifications";
    case sys::BusChannel::ServiceEvtmgrNotifications:
        return "ServiceEvtmgrNotifications";
    case sys::BusChannel::PhoneModeChanges:
        return "PhoneModeChanges";
    case sys::BusChannel::BluetoothModeChanges:
        return "BluetoothModeChanges";
    case sys::BusChannel::BluetoothNotifications:
        return "BluetoothNotifications";
    case sys::BusChannel::PhoneLockChanges:
        return "PhoneLockChanges";
    }
    return "";
}
