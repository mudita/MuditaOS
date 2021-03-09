// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCalendar.hpp"
#include "DialogMetadataMessage.hpp"
#include "windows/CalendarMainWindow.hpp"
#include "windows/DayEventsWindow.hpp"
#include "windows/CalendarEventsOptionsWindow.hpp"
#include "windows/AllEventsWindow.hpp"
#include "windows/EventDetailWindow.hpp"
#include "windows/NewEditEventWindow.hpp"
#include "windows/CustomRepeatWindow.hpp"
#include "windows/EventReminderWindow.hpp"
#include "NoEvents.hpp"
#include "Dialog.hpp"
#include <time/time_conversion.hpp>
#include <module-db/queries/calendar/QueryEventsAdd.hpp>

#include <service-db/DBServiceAPI.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>

namespace app
{
    const std::map<Reminder, const char *> ApplicationCalendar::reminderOptions = {
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

    const std::map<Repeat, const char *> ApplicationCalendar::repeatOptions = {
        {Repeat::never, "app_calendar_repeat_never"},
        {Repeat::daily, "app_calendar_repeat_daily"},
        {Repeat::weekly, "app_calendar_repeat_weekly"},
        {Repeat::biweekly, "app_calendar_repeat_two_weeks"},
        {Repeat::monthly, "app_calendar_repeat_month"},
        {Repeat::yearly, "app_calendar_repeat_year"}};

    ApplicationCalendar::ApplicationCalendar(std::string name,
                                             std::string parent,
                                             StartInBackground startInBackground,
                                             uint32_t stackDepth,
                                             sys::ServicePriority priority)
        : Application(name, parent, startInBackground, stackDepth, priority)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        addActionReceiver(manager::actions::ShowReminder, [this](auto &&data) {
            switchWindow(style::window::calendar::name::event_reminder_window, std::move(data));
            return actionHandled();
        });
    }

    sys::MessagePointer ApplicationCalendar::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (retMsg && (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            LOG_DEBUG("Received notification");
            // window-specific actions
            if (msg->interface == db::Interface::Name::Events) {
                for (auto &[name, window] : windowsStack.windows) {
                    window->onDatabaseMessage(msg);
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        }

        // this variable defines whether message was processed.
        bool handled = false;
        // handle database response
        if (resp != nullptr) {
            handled = true;
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
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
        using namespace style::window::calendar::name;

        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CalendarMainWindow>(app, name);
        });
        windowsFactory.attach(day_events_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DayEventsWindow>(app);
        });
        windowsFactory.attach(no_events_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NoEvents>(app, name);
        });
        windowsFactory.attach(style::window::calendar::name::events_options,
                              [](Application *app, const std::string &name) {
                                  return std::make_unique<gui::CalendarEventsOptions>(app);
                              });
        windowsFactory.attach(dialog_yes_no, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, dialog_yes_no);
        });
        windowsFactory.attach(all_events_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AllEventsWindow>(app, all_events_window);
        });
        windowsFactory.attach(details_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::EventDetailWindow>(app, details_window);
        });
        windowsFactory.attach(new_edit_event, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NewEditEventWindow>(app, new_edit_event);
        });
        windowsFactory.attach(custom_repeat_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CustomRepeatWindow>(app, custom_repeat_window);
        });
        windowsFactory.attach(event_reminder_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::EventReminderWindow>(app, event_reminder_window);
        });

        attachPopups({gui::popup::ID::Volume, gui::popup::ID::PhoneModes});
    }

    void ApplicationCalendar::destroyUserInterface()
    {}

    void ApplicationCalendar::switchToNoEventsWindow(const std::string &title, const TimePoint &dateFilter)
    {
        if (equivalentWindow == EquivalentWindow::DayEventsWindow) {
            popToWindow(gui::name::window::main_window);
        }
        if (equivalentWindow == EquivalentWindow::AllEventsWindow) {
            popToWindow(gui::name::window::main_window);
        }

        LOG_DEBUG("Switch to no events window");

        auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
            title,
            "phonebook_empty_grey_circle_W_G",
            utils::localize.get("app_calendar_no_events_information"),
            "",
            [=]() -> bool {
                LOG_DEBUG("Switch to new event window");
                auto event       = std::make_shared<EventsRecord>();
                event->date_from = dateFilter;
                event->date_till = dateFilter + std::chrono::hours(utils::time::Locale::max_hour_24H_mode) +
                                   std::chrono::minutes(utils::time::Locale::max_minutes);

                auto eventData = std::make_unique<EventRecordData>(std::move(event));
                eventData->setDescription(style::window::calendar::new_event);
                switchWindow(
                    style::window::calendar::name::new_edit_event, gui::ShowMode::GUI_SHOW_INIT, std::move(eventData));
                return true;
            }});

        switchWindow(style::window::calendar::name::no_events_window, std::move(metaData));
    }

} /* namespace app */
