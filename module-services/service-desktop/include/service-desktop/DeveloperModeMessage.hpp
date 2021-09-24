// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
