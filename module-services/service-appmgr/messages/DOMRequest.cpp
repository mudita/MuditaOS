// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DOMRequest.hpp"

#include <service-desktop/DeveloperModeMessage.hpp>

namespace app::manager
{

    DOMRequest::DOMRequest(ApplicationName sender, std::unique_ptr<sdesktop::Event> event)
        : sdesktop::developerMode::DeveloperModeRequest(std::move(event)), sender(std::move(sender))
    {}

    DOMRequest::DOMRequest(const DOMRequest &dom)
        : sdesktop::developerMode::DeveloperModeRequest(std::make_unique<sdesktop::Event>(*dom.event)),
          sender(std::move(dom.sender))
    {}
} // namespace app::manager
