﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Actions.hpp"
#include "messages/Message.hpp"
#include "model/ApplicationManager.hpp"

#include <module-sys/Service/Service.hpp>
#include <SwitchData.hpp>

#include <module-utils/i18n/i18n.hpp>

#include <memory>
#include <string>

namespace sys
{
    class Service;
} // namespace sys

namespace app::manager
{
    enum class OnSwitchBehaviour
    {
        Close,
        RunInBackground
    };

    class Controller
    {
      public:
        Controller() = delete;

        static auto applicationInitialised(sys::Service *sender,
                                           StartupStatus status,
                                           StartInBackground startInBackground) -> bool;
        static auto sendAction(sys::Service *sender,
                               actions::ActionId actionId,
                               actions::ActionParamsPtr &&data     = nullptr,
                               OnSwitchBehaviour onSwitchBehaviour = OnSwitchBehaviour::Close) -> bool;
        static auto switchBack(sys::Service *sender, std::unique_ptr<SwitchBackRequest> msg = nullptr) -> bool;
        static auto confirmSwitch(sys::Service *sender) -> bool;
        static auto closeApplication(sys::Service *sender, const ApplicationName &name) -> bool;
        static auto confirmClose(sys::Service *sender) -> bool;
        static auto changeDisplayLanguage(sys::Service *sender, const Language &language) -> bool;
        static auto changeInputLanguage(sys::Service *sender, const Language &language) -> bool;
        static auto changePowerSaveMode(sys::Service *sender) -> bool;
        static auto preventBlockingDevice(sys::Service *sender) -> bool;
    };
} // namespace app::manager
