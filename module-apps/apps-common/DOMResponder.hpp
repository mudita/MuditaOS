// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-desktop/DesktopEvent.hpp>
#include <Service/MessageForward.hpp>
#include <string>

namespace gui
{
    class Item;
}

namespace sdesktop::developerMode
{
    class Event;
}

namespace app
{
    /// This class essentially hides implementation of: ServiceDesktop mechanics and DOM serialization from Application
    /// gets event data
    /// creates DOM and enapsulates in it event data
    /// builds response to be used with ServiceDesktop response type
    class DOMResponder
    {
        std::string name;
        gui::Item &item;
        std::unique_ptr<sdesktop::Event> event;
        /// uses gui::Item serializer to create DOM dump
        void createDOM();

      public:
        DOMResponder(const std::string &, gui::Item &, std::unique_ptr<sdesktop::Event> event);
        /// creates response used to inform ServiceDesktop with DOM dump encapsulated
        /// requires use of createDOM first
        [[nodiscard]] auto build() -> std::shared_ptr<sys::Message>;
    };

}; // namespace app
