// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DOMResponder.hpp"
#include "service-desktop/DeveloperModeMessage.hpp"
#include <service-appmgr/messages/DOMRequest.hpp>
#include <module-gui/gui/dom/Item2JsonSerializer.hpp>
#include <memory>
#include <Item.hpp>
#include <time/ScopedTime.hpp>
#include <endpoints/developerMode/event/DomRequest.hpp>

namespace app
{
    DOMResponder::DOMResponder(const std::string &name, gui::Item &item, std::unique_ptr<sdesktop::Event> event)
        : name(name), item(item), event(std::move(event))
    {}

    [[nodiscard]] auto DOMResponder::build() -> std::shared_ptr<sys::Message>
    {
        createDOM();
        return std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
    }

    void DOMResponder::createDOM()
    {
        auto t          = utils::time::Scoped("Time to build dom");
        auto serializer = gui::Item2JsonSerializer();
        serializer.traverse(item);
        auto evt = std::make_unique<sdesktop::developerMode::DomRequestEvent>(*event);
        evt->setJson(serializer.get());
        event = std::move(evt);
    }
} // namespace app
