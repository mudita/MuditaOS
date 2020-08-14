#include "CalendarEventsOptionsWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "Dialog.hpp"
#include <Utils.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-db/queries/calendar/QueryEventsRemove.hpp>
#include <module-apps/application-calendar/data/CalendarData.hpp>

namespace gui
{

    CalendarEventsOptions::CalendarEventsOptions(app::Application *app)
        : OptionWindow(app, style::window::calendar::name::events_options)
    {
        buildInterface();
    }

    auto CalendarEventsOptions::eventsOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;
        options.emplace_back(gui::Option{utils::localize.get("app_calendar_options_edit"), [=](gui::Item &item) {
                                             LOG_INFO("Switch to edit window");
                                             auto rec  = std::make_unique<EventsRecord>(*eventRecord);
                                             auto data = std::make_unique<EventRecordData>(std::move(rec));
                                             data->setDescription("Edit");
                                             data->setWindowName(goBackWindowName);
                                             application->switchWindow(style::window::calendar::name::new_edit_event,
                                                                       std::move(data));
                                             return true;
                                         }});
        options.emplace_back(gui::Option{utils::localize.get("app_calendar_options_delete"),
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

        eventRecord      = item->getData();
        goBackWindowName = item->getWindowName();
        clearOptions();
        addOptions(eventsOptionsList());
        return true;
    }

    auto CalendarEventsOptions::eventDelete() -> bool
    {
        LOG_DEBUG("Switch to delete event window");
        auto dialog = dynamic_cast<gui::DialogYesNo *>(
            this->application->getWindow(style::window::calendar::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            LOG_INFO("Delete calendar event %d", static_cast<int>(eventRecord->ID));
            DBServiceAPI::GetQuery(
                application, db::Interface::Name::Events, std::make_unique<db::query::events::Remove>(eventRecord->ID));
            auto data = make_unique<PrevWindowData>();
            data->setData(PrevWindowData::PrevWindow::Delete);
            application->switchWindow(goBackWindowName, std::move(data));
            return true;
        };
        meta.text  = utils::localize.get("app_calendar_event_delete_confirmation");
        meta.title = eventRecord->title;
        meta.icon  = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        this->application->switchWindow(dialog->getName());
        return true;
    }
} // namespace gui
