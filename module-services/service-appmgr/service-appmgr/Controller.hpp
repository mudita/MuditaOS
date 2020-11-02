// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Actions.hpp"
#include "Message.hpp"
#include "model/ApplicationManager.hpp"

#include <module-sys/Service/Service.hpp>
#include <SwitchData.hpp>

#include <i18/i18.hpp>

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

        static auto sendAction(sys::Service *sender, actions::ActionId actionId, actions::ActionParamsPtr &&data)
            -> bool;
        static auto applicationInitialised(sys::Service *sender,
                                           StartupStatus status,
                                           StartInBackground startInBackground) -> bool;
        static auto switchApplication(sys::Service *sender,
                                      const ApplicationName &applicationName,
                                      const std::string &windowName,
                                      std::unique_ptr<gui::SwitchData> data = nullptr) -> bool;
        static auto switchBack(sys::Service *sender, std::unique_ptr<SwitchBackRequest> msg = nullptr) -> bool;
        static auto closeApplication(sys::Service *sender, const ApplicationName &name) -> bool;
        static auto changeDisplayLanguage(sys::Service *sender, utils::Lang language) -> bool;
        static auto changeInputLanguage(sys::Service *sender, utils::Lang language) -> bool;
        static auto stopApplicationManager(sys::Service *sender) -> bool;
        static auto preventBlockingDevice(sys::Service *sender) -> bool;
        static auto confirmSwitch(sys::Service *sender) -> bool;
        static auto confirmClose(sys::Service *sender) -> bool;
        static auto lockScreen(sys::Service *sender) -> bool;
        static auto unlockScreen(sys::Service *sender) -> bool;
    };
} // namespace app::manager
