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
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>

namespace app
{

    ApplicationAlarmClock::ApplicationAlarmClock(std::string name,
                                                 std::string parent,
                                                 sys::phone_modes::PhoneMode phoneMode,
                                                 sys::bluetooth::BluetoothMode bluetoothMode,
                                                 uint32_t stackDepth,
                                                 sys::ServicePriority priority)
        : Application(name, parent, phoneMode, bluetoothMode, false, stackDepth, priority)
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
            // window-specific actions
            if (msg->interface == db::Interface::Name::AlarmEvents) {
                for (auto &[name, window] : windowsStack.windows) {
                    window->onDatabaseMessage(msg);
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        }

        return handleAsyncResponse(resp);
    }

    sys::ReturnCodes ApplicationAlarmClock::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();
        return ret;
    }

    void ApplicationAlarmClock::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            auto alarmsRepository = std::make_unique<alarmClock::AlarmsDBRepository>(app);
            auto alarmsProvider   = std::make_shared<alarmClock::AlarmsModel>(app, std::move(alarmsRepository));
            auto presenter        = std::make_unique<alarmClock::AlarmClockMainWindowPresenter>(alarmsProvider);
            return std::make_unique<alarmClock::AlarmClockMainWindow>(app, std::move(presenter));
        });

        auto rRulePresenter = std::make_shared<alarmClock::AlarmRRulePresenter>();

        windowsFactory.attach(style::alarmClock::window::name::newEditAlarm,
                              [rRulePresenter](ApplicationCommon *app, const std::string &name) {
                                  auto alarmsRepository = std::make_unique<alarmClock::AlarmsDBRepository>(app);
                                  auto alarmsProvider   = std::make_shared<alarmClock::NewEditAlarmModel>(
                                      app, rRulePresenter, std::move(alarmsRepository), !stm::api::isTimeFormat12h());
                                  auto presenter =
                                      std::make_unique<alarmClock::AlarmClockEditWindowPresenter>(alarmsProvider);
                                  return std::make_unique<alarmClock::NewEditAlarmWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(style::alarmClock::window::name::customRepeat,
                              [rRulePresenter](ApplicationCommon *app, const std::string &name) {
                                  auto alarmsProvider =
                                      std::make_shared<alarmClock::CustomRepeatModel>(app, rRulePresenter);
                                  auto presenter =
                                      std::make_unique<alarmClock::CustomRepeatWindowPresenter>(alarmsProvider);
                                  return std::make_unique<alarmClock::CustomRepeatWindow>(app, std::move(presenter));
                              });
        windowsFactory.attach(utils::translate("app_alarm_clock_options_title"),
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::OptionWindow>(app, name);
                              });

        windowsFactory.attach(style::alarmClock::window::name::dialogYesNo,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::DialogYesNo>(app, name);
                              });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationAlarmClock::destroyUserInterface()
    {}

} /* namespace app */
