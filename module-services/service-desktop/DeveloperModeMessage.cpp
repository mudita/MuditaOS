// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <service-desktop/DeveloperModeMessage.hpp>

namespace sdesktop::developerMode
{
    DeveloperModeRequest::DeveloperModeRequest(std::unique_ptr<Event> event)
        : sys::DataMessage(MessageType::DeveloperModeRequest), event(std::move(event))
    {}

    DeveloperModeRequest::DeveloperModeRequest() : sys::DataMessage(MessageType::DeveloperModeRequest)
    {}
} // namespace sdesktop::developerMode
