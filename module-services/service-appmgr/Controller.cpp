// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/Controller.hpp"

#include "service-appmgr/model/ApplicationManager.hpp"

#include <Service/Service.hpp>

#include <utility> // for move

namespace app::manager
{
    namespace
    {
        void setOnSwitchBehaviour(actions::ActionParamsPtr &data, OnSwitchBehaviour onSwitchBehaviour) noexcept
        {
            if (data) {
                data->disableAppClose = (onSwitchBehaviour == OnSwitchBehaviour::RunInBackground);
            }
        }
    } // namespace

    auto Controller::applicationInitialised(sys::Service *sender,
                                            StartupStatus status,
                                            StartInBackground startInBackground) -> bool
    {
        auto msg = std::make_shared<app::manager::ApplicationInitialised>(sender->GetName(), status, startInBackground);
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::sendAction(sys::Service *sender,
                                actions::ActionId actionId,
                                actions::ActionParamsPtr &&data,
                                OnSwitchBehaviour onSwitchBehaviour) -> bool
    {
        setOnSwitchBehaviour(data, onSwitchBehaviour);
        auto msg = std::make_shared<app::manager::ActionRequest>(sender->GetName(), actionId, std::move(data));
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::switchBack(sys::Service *sender, std::unique_ptr<SwitchBackRequest> msg) -> bool
    {
        std::shared_ptr<SwitchBackRequest> switchMsg =
            msg ? std::move(msg) : std::make_shared<app::manager::SwitchBackRequest>(sender->GetName());
        return sender->bus.sendUnicast(switchMsg, ApplicationManager::ServiceName);
    }

    auto Controller::confirmSwitch(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::SwitchConfirmation>(sender->GetName());
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::closeApplication(sys::Service *sender, const ApplicationName &name) -> bool
    {
        auto msg = std::make_shared<app::manager::ApplicationCloseRequest>(sender->GetName(), name);
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::confirmClose(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::CloseConfirmation>(sender->GetName());
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::changeDisplayLanguage(sys::Service *sender, const Language &language) -> bool
    {
        auto msg = std::make_shared<app::manager::DisplayLanguageChangeRequest>(sender->GetName(), language);
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::changeInputLanguage(sys::Service *sender, const Language &language) -> bool
    {
        auto msg = std::make_shared<app::manager::InputLanguageChangeRequest>(sender->GetName(), language);
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::changeAutomaticDateAndTimeIsOn(sys::Service *sender, bool isOn) -> bool
    {
        auto msg = std::make_shared<app::manager::AutomaticDateAndTimeIsOnChangeRequest>(isOn);
        return sender->bus.sendUnicast(std::move(msg), ApplicationManager::ServiceName);
    }

    auto Controller::changeAutomaticTimeZoneIsOn(sys::Service *sender, bool isOn) -> bool
    {
        auto msg = std::make_shared<app::manager::AutomaticTimeZoneIsOnChangeRequest>(isOn);
        return sender->bus.sendUnicast(std::move(msg), ApplicationManager::ServiceName);
    }

    auto Controller::changeTimeFormat(sys::Service *sender, utils::time::Locale::TimeFormat timeFormat) -> bool
    {
        auto msg = std::make_shared<app::manager::TimeFormatChangeRequest>(timeFormat);
        return sender->bus.sendUnicast(std::move(msg), ApplicationManager::ServiceName);
    }

    auto Controller::changeDateFormat(sys::Service *sender, utils::time::Locale::DateFormat dateFormat) -> bool
    {
        auto msg = std::make_shared<app::manager::DateFormatChangeRequest>(dateFormat);
        return sender->bus.sendUnicast(std::move(msg), ApplicationManager::ServiceName);
    }

    auto Controller::preventBlockingDevice(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::PreventBlockingRequest>(sender->GetName());
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }

    auto Controller::changePowerSaveMode(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::PowerSaveModeInitRequest>(sender->GetName());
        return sender->bus.sendUnicast(msg, ApplicationManager::ServiceName);
    }
} // namespace app::manager
