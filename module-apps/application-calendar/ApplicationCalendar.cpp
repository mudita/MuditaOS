#include "ApplicationCalendar.hpp"
#include "windows/CalendarMainWindow.hpp"
#include "windows/DayEventsWindow.hpp"
#include "windows/CalendarEventsOptionsWindow.hpp"
#include "windows/AllEventsWindow.hpp"
#include "windows/EventDetailWindow.hpp"
#include "windows/NewEditEventWindow.hpp"
#include "windows/CustomRepeatWindow.hpp"
#include "NoEvents.hpp"
#include "Dialog.hpp"
#include <time/time_conversion.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-db/queries/calendar/QueryEventsAdd.hpp>
#include <module-services/service-db/messages/QueryMessage.hpp>

namespace app
{
    const map<Reminder, const char *> ApplicationCalendar::reminderOptions = {
        {Reminder::never, "app_calendar_reminder_never"},
        {Reminder::event_time, "app_calendar_reminder_event_time"},
        {Reminder::five_min_before, "app_calendar_reminder_5_min_before"},
        {Reminder::fifteen_min_before, "app_calendar_reminder_15_min_before"},
        {Reminder::thirty_min_before, "app_calendar_reminder_30_min_before"},
        {Reminder::one_hour_before, "app_calendar_reminder_1_hour_before"},
        {Reminder::two_hour_before, "app_calendar_reminder_2_hour_before"},
        {Reminder::one_day_before, "app_calendar_reminder_1_day_before"},
        {Reminder::two_days_before, "app_calendar_reminder_2_days_before"},
        {Reminder::one_week_before, "app_calendar_reminder_1_week_before"}};

    const map<Repeat, const char *> ApplicationCalendar::repeatOptions = {
        {Repeat::never, "app_calendar_repeat_never"},
        {Repeat::daily, "app_calendar_repeat_daily"},
        {Repeat::weekly, "app_calendar_repeat_weekly"},
        {Repeat::two_weeks, "app_calendar_repeat_two_weeks"},
        {Repeat::month, "app_calendar_repeat_month"},
        {Repeat::year, "app_calendar_repeat_year"},
        {Repeat::custom, "app_calendar_repeat_custom"}};

    ApplicationCalendar::ApplicationCalendar(std::string name,
                                             std::string parent,
                                             uint32_t stackDepth,
                                             sys::ServicePriority priority)
        : Application(name, parent, false, stackDepth, priority)
    {}

    sys::Message_t ApplicationCalendar::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (retMsg && (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }
        // this variable defines whether message was processed.
        bool handled = false;
        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBQuery: {
                if (getCurrentWindow()->onDatabaseMessage(resp))
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            } break;
            default:
                break;
            }
        }
        if (handled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    sys::ReturnCodes ApplicationCalendar::InitHandler()
    {
        utils::time::Timestamp timestamp;
        applicationStartTime = timestamp.getTime();
        auto ret             = Application::InitHandler();
        createUserInterface();
        return ret;
    }

    sys::ReturnCodes ApplicationCalendar::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationCalendar::createUserInterface()
    {

        windows.insert(std::pair<std::string, gui::AppWindow *>(
            gui::name::window::main_window, new gui::CalendarMainWindow(this, gui::name::window::main_window)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::day_events_window,
            new gui::DayEventsWindow(this, style::window::calendar::name::day_events_window)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::no_events_window,
            new gui::NoEvents(this, style::window::calendar::name::no_events_window, gui::NoEvents::Meta())));
        windows.insert(std::pair<std::string, gui::AppWindow *>(style::window::calendar::name::events_options,
                                                                new gui::CalendarEventsOptions(this)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::dialog_yes_no,
            new gui::DialogYesNo(this, style::window::calendar::name::dialog_yes_no)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::all_events_window,
            new gui::AllEventsWindow(this, style::window::calendar::name::all_events_window)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::details_window,
            new gui::EventDetailWindow(this, style::window::calendar::name::details_window)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::new_edit_event,
            new gui::NewEditEventWindow(this, style::window::calendar::name::new_edit_event)));
        windows.insert(std::pair<std::string, gui::AppWindow *>(
            style::window::calendar::name::custom_repeat_window,
            new gui::CustomRepeatWindow(this, style::window::calendar::name::custom_repeat_window)));
    }

    void ApplicationCalendar::destroyUserInterface()
    {}

    void ApplicationCalendar::switchToNoEventsWindow(const std::string &title,
                                                     const TimePoint &dateFilter,
                                                     const std::string &goBackWindow)
    {
        auto dialog = dynamic_cast<gui::NoEvents *>(getWindow(style::window::calendar::name::no_events_window));
        assert(dialog != nullptr);
        auto meta  = dialog->meta;
        meta.text  = "app_calendar_no_events_information";
        meta.title = title;
        meta.icon  = "phonebook_empty_grey_circle_W_G";

        meta.action = [=]() -> bool {
            LOG_DEBUG("Switch to new event window");
            std::unique_ptr<EventRecordData> eventData = std::make_unique<EventRecordData>();
            eventData->setDescription(style::window::calendar::new_event);
            auto event       = std::make_shared<EventsRecord>();
            event->date_from = dateFilter;
            event->date_till = dateFilter;
            eventData->setData(event);
            eventData->setWindowName(goBackWindow);
            switchWindow(
                style::window::calendar::name::new_edit_event, gui::ShowMode::GUI_SHOW_INIT, std::move(eventData));
            return true;
        };
        dialog->update(meta);
        switchWindow(gui::name::window::main_window);
        switchWindow(dialog->getName());
        LOG_DEBUG("Switch to no events window");
    }

} /* namespace app */
