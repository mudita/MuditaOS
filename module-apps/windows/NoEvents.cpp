// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoEvents.hpp"
#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"
#include "log/log.hpp"
#include <service-appmgr/ApplicationManager.hpp>
#include <i18/i18.hpp>
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>

using namespace gui;

namespace style
{
    namespace arrow
    {
        const inline uint32_t x = 30;
        const inline uint32_t y = 62;
    } // namespace arrow

    namespace cross
    {
        const inline uint32_t x = 48;
        const inline uint32_t y = 55;
    } // namespace cross
} // namespace style

NoEvents::NoEvents(app::Application *app, const std::string &name) : gui::Dialog(app, name)
{}

void NoEvents::onBeforeShow(ShowMode mode, SwitchData *data)
{
    Dialog::onBeforeShow(mode, data);
    auto metadata = dynamic_cast<DialogMetadataMessage *>(data);
    if (metadata != nullptr) {
        auto foo      = metadata->get().action;
        inputCallback = [foo](Item &, const InputEvent &inputEvent) -> bool {
            if (foo && inputEvent.isShortPress() && inputEvent.is(KeyCode::KEY_LEFT)) {
                return foo();
            }
            return false;
        };
    }
}
