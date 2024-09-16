// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <service-desktop/DesktopEvent.hpp>

namespace sdesktop::developerMode
{
    class DeveloperModeRequest : public sys::DataMessage
    {
      public:
        std::unique_ptr<sdesktop::Event> event;
        explicit DeveloperModeRequest(std::unique_ptr<sdesktop::Event> event);
        DeveloperModeRequest();
    };
} // namespace sdesktop::developerMode
