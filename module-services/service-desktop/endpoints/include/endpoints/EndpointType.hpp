// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        usbSecurity
    };

    inline constexpr auto lastEndpoint = static_cast<int>(EndpointType::usbSecurity);

} // namespace sdesktop::endpoints
