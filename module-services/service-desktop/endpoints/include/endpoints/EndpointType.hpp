// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <magic_enum.hpp>

namespace sdesktop::endpoints
{
    enum class EndpointType
    {
        Invalid = 0,
        DeviceInfo,
        Update,
        FilesystemUpload,
        Backup,
        Restore,
        Factory,
        Contacts,
        Messages,
        Calllog,
        CalendarEventsPlaceholder,
        DeveloperMode,
        Bluetooth,
        UsbSecurity,
        Outbox,
        Reboot,
        TimeSync
    };

    inline constexpr auto lastEndpoint = magic_enum::enum_count<EndpointType>() - 1;

} // namespace sdesktop::endpoints
