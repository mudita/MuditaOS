// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-test/ServiceTest.hpp"
#include "Timers/TimerFactory.hpp"
#include "application-test/include/application-test/ApplicationTest.hpp"
#include "service-appmgr/Controller.hpp"

namespace service::test
{

    static std::uint32_t stackSize       = 2048;
    constexpr auto setting_private_value = "private value";

    ServiceTest::ServiceTest() : sys::Service(service::name::service_test, "", stackSize)
    {
        LOG_INFO("[ServiceTest] Initializing");
    }

    sys::ReturnCodes ServiceTest::InitHandler()
    {
        settings.init(service::ServiceProxy(shared_from_this()));
        // set some value to settings
        settings.setValue(setting_private_value, "it works");
        // log this value in logs
        // it will output: "Now we can use settings! it works"
        LOG_INFO("Now we can use settings! %s", settings.getValue(setting_private_value).c_str());

        // this code will create a periodic timer which will in 2 seconds:
        // - log "timers are awesome, periodic timer is active: 1" around each second
        // - trigger popup to the test application on the timer
        th = sys::TimerFactory::createPeriodicTimer(
            this, "my  periodic timer", std::chrono::milliseconds(2 * 1000), [this](sys::Timer &t) {
                LOG_INFO("timers are avesome, single shot timer is active: %d", t.isActive());
                if (appStarted == false) {
                    LOG_INFO("Launching TestApplication");
                    app::manager::Controller::sendAction(
                        this,
                        app::manager::actions::Launch,
                        std::make_unique<app::ApplicationLaunchData>(app::application_test));
                    // not really started but.. will be good enough
                    appStarted = true;
                    return;
                }
                LOG_INFO("send action to the test appliaction!");
                app::manager::Controller::sendAction(
                    this,
                    app::manager::actions::ShowPopup,
                    std::make_unique<gui::PopupRequestParams>(gui::popup::ID::AppTestPopup));
                t.stop();
            });
        th.start();
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTest::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTest::SwitchPowerModeHandler(const sys::ServicePowerMode /*mode*/)
    {
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ServiceTest::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }
} // namespace service::test
