// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationManager.hpp" // for ApplicationHandle, ApplicationHandle::Name

#include <SwitchData.hpp>
#include <i18/i18.hpp>
#include <module-sys/Service/Service.hpp>
#include <service-appmgr/messages/APMMessage.hpp>

#include <memory>
#include <string>

namespace sys
{
    class Service;
} // namespace sys

namespace app::manager
{
    class Controller
    {
      public:
        Controller() = delete;

        static auto sendAction(sys::Service *sender, Action &&action) -> bool;
        static auto registerApplication(sys::Service *sender, bool status, bool startBackground) -> bool;
        static auto switchApplication(sys::Service *sender,
                                      const ApplicationHandle::Name &applicationName,
                                      const std::string &windowName,
                                      std::unique_ptr<gui::SwitchData> data = nullptr) -> bool;
        static auto switchBack(sys::Service *sender, std::unique_ptr<APMSwitchPrevApp> msg = nullptr) -> bool;
        static auto closeApplication(sys::Service *sender, const ApplicationHandle::Name &name) -> bool;
        static auto changeLanguage(sys::Service *sender, utils::Lang language) -> bool;
        static auto changePowerSaveMode(sys::Service *sender) -> bool;
        static auto stopApplicationManager(sys::Service *sender) -> bool;
        static auto preventBlockingDevice(sys::Service *sender) -> bool;
        static auto confirmSwitch(sys::Service *sender) -> bool;
        static auto confirmClose(sys::Service *sender) -> bool;
    };
} // namespace app::manager
