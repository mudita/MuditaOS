// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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
        outbox
    };

    inline constexpr auto lastEndpoint = static_cast<int>(EndpointType::outbox);

} // namespace sdesktop::endpoints
