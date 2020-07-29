#include "ApplicationCalendar.hpp"
#include "windows/CalendarMainWindow.hpp"
#include "windows/DayEventsWindow.hpp"
#include "windows/CalendarEventsOptionsWindow.hpp"
#include "windows/AllEventsWindow.hpp"
#include "windows/EventDetailWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "NoEvents.hpp"
#include "Dialog.hpp"
#include <map>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include <module-db/queries/calendar/QueryEventsGet.hpp>
#include <module-db/queries/calendar/QueryEventsAdd.hpp>
#include <module-db/queries/calendar/QueryEventsEdit.hpp>
#include <module-db/queries/calendar/QueryEventsGetAll.hpp>
#include <module-db/queries/calendar/QueryEventsRemove.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <messages/QueryMessage.hpp>

namespace app
{

    ApplicationCalendar::ApplicationCalendar(std::string name,
                                             std::string parent,
                                             uint32_t stackDepth,
                                             sys::ServicePriority priority)
        : Application(name, parent, false, stackDepth, priority)
    {}

    sys::Message_t ApplicationCalendar::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return Application::DataReceivedHandler(msgl);
    }

    sys::ReturnCodes ApplicationCalendar::InitHandler()
    {
        auto ret = Application::InitHandler();
        EventsRecord event(EventsTableRow{{1}, "TEST", "TEST", 191020142, 191020153, 1, 2, 1});
        EventsRecord event2(EventsTableRow{{2}, "TEST2", "TEST2", 191020152, 191020163, 1, 2, 1});
        DBServiceAPI::GetQuery(this, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(event));
        DBServiceAPI::GetQuery(this, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(event));
        DBServiceAPI::GetQuery(this, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(event2));
        DBServiceAPI::GetQuery(this, db::Interface::Name::Events, std::make_unique<db::query::events::Add>(event2));
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
    }

    void ApplicationCalendar::destroyUserInterface()
    {}

} /* namespace app */
