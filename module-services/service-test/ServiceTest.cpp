// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-test/ServiceTest.hpp"
#include "Timers/TimerFactory.hpp"

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

        // this code will create periodic system timer which will log:
        // "timers are avesome, periodic timer is active: 1" around each second
        th = sys::TimerFactory::createPeriodicTimer(
            this, "my  periodic timer", std::chrono::milliseconds(1000), [](sys::Timer &t) {
                LOG_INFO("timers are avesome, periodic timer is active: %d", t.isActive());
            });
        th.start();
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTest::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceTest::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ServiceTest::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }
} // namespace service::test
