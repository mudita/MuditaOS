// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseMessage.hpp"
#include "service-desktop/DeveloperModeMessage.hpp"
#include <json11.hpp>

namespace app::manager
{
    class DOMRequest : public sdesktop::developerMode::DeveloperModeRequest
    {
        ApplicationName sender;

      public:
        DOMRequest(ApplicationName sender, std::unique_ptr<sdesktop::Event> event);
        [[nodiscard]] auto getEvent()
        {
            return std::move(event);
        }
        [[nodiscard]] auto getSenderName() const
        {
            return sender;
        }
        DOMRequest(const DOMRequest &dom);
    };
} // namespace app::manager
