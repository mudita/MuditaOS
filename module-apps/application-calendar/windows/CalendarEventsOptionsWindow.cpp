#include "CalendarEventsOptionsWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "Dialog.hpp"
#include <Utils.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-db/queries/calendar/QueryEventsRemove.hpp>

namespace gui
{

    CalendarEventsOptions::CalendarEventsOptions(app::Application *app)
        : OptionWindow(app, style::window::calendar::name::events_options)
    {
        buildInterface();
        this->addOptions(eventsOptionsList());
    }

    auto CalendarEventsOptions::eventsOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;
        options.emplace_back(gui::Option{utils::localize.get("app_calendar_options_edit"), [=](gui::Item &item) {
                                             LOG_INFO("Switch to edit window");
                                             std::unique_ptr<gui::SwitchData> data = std::make_unique<SwitchData>();
                                             data->setDescription("Edit");
                                             application->switchWindow(style::window::calendar::name::new_edit_event,
                                                                       gui::ShowMode::GUI_SHOW_INIT,
                                                                       std::move(data));
                                             return true;
                                         }});
        options.emplace_back(gui::Option{utils::localize.get("app_calendar_options_delete"),
                                         [=](gui::Item &item) { return eventDelete(); }});
        return options;
    }

    auto CalendarEventsOptions::eventDelete() -> bool
    {
        LOG_DEBUG("Switch to delete event window");
        auto dialog = dynamic_cast<gui::DialogYesNo *>(
            this->application->getWindow(style::window::calendar::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            LOG_INFO("Detele calendar event");
            uint32_t mockID = 2;
            DBServiceAPI::GetQuery(
                application, db::Interface::Name::Events, std::make_unique<db::query::events::Remove>(mockID));
            return true;
        };
        meta.text  = utils::localize.get("app_calendar_event_delete_confirmation");
        meta.title = "Football with folks";
        meta.icon  = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        this->application->switchWindow(dialog->getName());
        return true;
    }
} // namespace gui
