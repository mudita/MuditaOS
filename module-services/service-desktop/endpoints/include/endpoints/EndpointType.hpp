// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <magic_enum.hpp>

namespace sdesktop::endpoints
{

    enum class EndpointType
    {
        invalid = 0,
        deviceInfo,
        update,
        filesystemUpload,
        backup,
        restore,
        factory,
        contacts,
        messages,
        calllog,
        calendarEventsPlaceholder,
        developerMode,
        bluetooth,
        usbSecurity,
        outbox,
        msc
    };

    inline constexpr auto lastEndpoint = magic_enum::enum_count<EndpointType>() - 1;

} // namespace sdesktop::endpoints
