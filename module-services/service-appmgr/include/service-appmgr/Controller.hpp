// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/Message.hpp>

#include <Service/Service.hpp>
#include <SwitchData.hpp>

#include <i18n/i18n.hpp>
#include <time/time_locale.hpp>

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

        static auto finish(sys::Service *sender) -> bool;
        static auto applicationInitialised(sys::Service *sender,
                                           StartupStatus status,
                                           StartInBackground startInBackground) -> bool;
        static auto sendAction(sys::Service *sender,
                               actions::ActionId actionId,
                               actions::ActionParamsPtr &&data     = nullptr,
                               OnSwitchBehaviour onSwitchBehaviour = OnSwitchBehaviour::RunInBackground) -> bool;
        static auto switchBack(sys::Service *sender, std::unique_ptr<SwitchBackRequest> msg = nullptr) -> bool;
        static auto confirmSwitch(sys::Service *sender) -> bool;
        static auto closeApplication(sys::Service *sender, const ApplicationName &name) -> bool;
        static auto finalizingClose(sys::Service *sender) -> bool;
        static auto confirmClose(sys::Service *sender) -> bool;
        static auto changeDisplayLanguage(sys::Service *sender, const Language &language) -> bool;
        static auto changeInputLanguage(sys::Service *sender, const Language &language) -> bool;
        static auto changeAutomaticDateAndTimeIsOn(sys::Service *sender, bool isOn) -> bool;
        static auto changeAutomaticTimeZoneIsOn(sys::Service *sender, bool isOn) -> bool;
        static auto changeTimeFormat(sys::Service *sender, utils::time::Locale::TimeFormat timeFormat) -> bool;
        static auto changeDateFormat(sys::Service *sender, utils::time::Locale::DateFormat dateFormat) -> bool;
        static auto changePowerSaveMode(sys::Service *sender) -> bool;
        static auto preventBlockingDevice(sys::Service *sender) -> bool;
        static auto requestNotifications(sys::Service *sender) -> bool;
    };
} // namespace app::manager
