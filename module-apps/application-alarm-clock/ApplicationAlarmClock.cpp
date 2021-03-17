// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationAlarmClock.hpp"
#include "application-alarm-clock/windows/AlarmClockMainWindow.hpp"
#include "application-alarm-clock/windows/NewEditAlarmWindow.hpp"
#include "application-alarm-clock/windows/CustomRepeatWindow.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/presenter/AlarmClockMainWindowPresenter.hpp"
#include "application-alarm-clock/presenter/CustomRepeatWindowPresenter.hpp"
#include "windows/Dialog.hpp"
#include "windows/AppWindow.hpp"
#include "windows/OptionWindow.hpp"
#include <module-services/service-db/service-db/DBNotificationMessage.hpp>
#include <module-services/service-db/service-db/QueryMessage.hpp>

namespace app
{

    ApplicationAlarmClock::ApplicationAlarmClock(std::string name,
                                                 std::string parent,
                                                 uint32_t stackDepth,
                                                 sys::ServicePriority priority)
        : Application(name, parent, false, stackDepth, priority)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    }

    sys::MessagePointer ApplicationAlarmClock::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (retMsg && (static_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            LOG_DEBUG("Received notification");
            // window-specific actions
            if (msg->interface == db::Interface::Name::Alarms) {
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
            return sys::msgHandled();
        }
        return sys::msgNotHandled();
    }

    sys::ReturnCodes ApplicationAlarmClock::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);
        return ret;
    }

    sys::ReturnCodes ApplicationAlarmClock::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationAlarmClock::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            auto alarmsRepository = std::make_unique<alarmClock::AlarmsDBRepository>(app);
            auto alarmsProvider   = std::make_shared<alarmClock::AlarmsModel>(app, std::move(alarmsRepository));
            auto presenter        = std::make_unique<alarmClock::AlarmClockMainWindowPresenter>(alarmsProvider);
            return std::make_unique<alarmClock::AlarmClockMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(
            style::alarmClock::window::name::newEditAlarm, [](Application *app, const std::string &name) {
                auto alarmsRepository = std::make_unique<alarmClock::AlarmsDBRepository>(app);
                auto alarmsProvider = std::make_shared<alarmClock::NewEditAlarmModel>(app, std::move(alarmsRepository));
                auto presenter      = std::make_unique<alarmClock::AlarmClockEditWindowPresenter>(alarmsProvider);
                return std::make_unique<alarmClock::NewEditAlarmWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(
            style::alarmClock::window::name::customRepeat, [](Application *app, const std::string &name) {
                auto alarmsRepository = std::make_unique<alarmClock::AlarmsDBRepository>(app);
                auto alarmsProvider = std::make_shared<alarmClock::CustomRepeatModel>(app, std::move(alarmsRepository));
                auto presenter      = std::make_unique<alarmClock::CustomRepeatWindowPresenter>(alarmsProvider);
                return std::make_unique<alarmClock::CustomRepeatWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(
            utils::localize.get("app_alarm_clock_options_title"),
            [](Application *app, const std::string &name) { return std::make_unique<gui::OptionWindow>(app, name); });

        windowsFactory.attach(
            style::alarmClock::window::name::dialogYesNo,
            [](Application *app, const std::string &name) { return std::make_unique<gui::DialogYesNo>(app, name); });

        attachPopups({gui::popup::ID::Volume});
    }

    void ApplicationAlarmClock::destroyUserInterface()
    {}

} /* namespace app */
