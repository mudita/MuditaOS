// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarEventsOptionsWindow.hpp"
#include "DialogMetadataMessage.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "Dialog.hpp"
#include <Utils.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <module-db/queries/calendar/QueryEventsRemove.hpp>
#include <module-apps/application-calendar/data/CalendarData.hpp>

namespace gui
{

    CalendarEventsOptions::CalendarEventsOptions(app::Application *app)
        : OptionWindow(app, style::window::calendar::name::events_options)
    {}

    auto CalendarEventsOptions::eventsOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;
        options.emplace_back(gui::Option{utils::translate("app_calendar_options_edit"), [=](gui::Item &item) {
                                             LOG_INFO("Switch to edit window");
                                             auto rec  = std::make_unique<EventsRecord>(*eventRecord);
                                             auto data = std::make_unique<EventRecordData>(std::move(rec));
                                             data->setDescription("Edit");
                                             application->switchWindow(style::window::calendar::name::new_edit_event,
                                                                       std::move(data));
                                             return true;
                                         }});
        options.emplace_back(gui::Option{utils::translate("app_calendar_options_delete"),
                                         [=](gui::Item &item) { return eventDelete(); }});
        return options;
    }

    auto CalendarEventsOptions::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<EventRecordData *>(data);
        if (item == nullptr) {
            return false;
        }

        eventRecord = item->getData();
        clearOptions();
        addOptions(eventsOptionsList());
        return true;
    }

    auto CalendarEventsOptions::eventDelete() -> bool
    {
        LOG_DEBUG("Switch to delete event window");

        auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
            eventRecord->title,
            "phonebook_contact_delete_trashcan",
            utils::translate("app_calendar_event_delete_confirmation"),
            "",
            [=]() -> bool {
                LOG_INFO("Delete calendar event %d", static_cast<int>(eventRecord->ID));
                DBServiceAPI::GetQuery(application,
                                       db::Interface::Name::Events,
                                       std::make_unique<db::query::events::Remove>(eventRecord->ID));
                const uint32_t numberOfIgnoredWindows = 3;
                application->returnToPreviousWindow(numberOfIgnoredWindows);
                return true;
            }});
        this->application->switchWindow(
            style::window::calendar::name::dialog_yes_no, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
        return true;
    }
} // namespace gui
