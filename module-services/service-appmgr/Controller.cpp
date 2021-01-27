// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/Controller.hpp"

#include "service-appmgr/model/ApplicationManager.hpp"

#include <module-sys/Service/Bus.hpp>
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
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::sendAction(sys::Service *sender,
                                actions::ActionId actionId,
                                actions::ActionParamsPtr &&data,
                                OnSwitchBehaviour onSwitchBehaviour) -> bool
    {
        setOnSwitchBehaviour(data, onSwitchBehaviour);
        auto msg = std::make_shared<app::manager::ActionRequest>(sender->GetName(), actionId, std::move(data));
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::switchBack(sys::Service *sender, std::unique_ptr<SwitchBackRequest> msg) -> bool
    {

        std::shared_ptr<SwitchBackRequest> switchMsg =
            msg ? std::move(msg) : std::make_shared<app::manager::SwitchBackRequest>(sender->GetName());
        return sys::Bus::SendUnicast(switchMsg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::confirmSwitch(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::SwitchConfirmation>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::closeApplication(sys::Service *sender, const ApplicationName &name) -> bool
    {
        auto msg = std::make_shared<app::manager::ApplicationCloseRequest>(sender->GetName(), name);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::confirmClose(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::CloseConfirmation>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeDisplayLanguage(sys::Service *sender, const Language &language) -> bool
    {
        auto msg = std::make_shared<app::manager::DisplayLanguageChangeRequest>(sender->GetName(), language);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeInputLanguage(sys::Service *sender, const Language &language) -> bool
    {
        auto msg = std::make_shared<app::manager::InputLanguageChangeRequest>(sender->GetName(), language);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeAutomaticDateAndTimeIsOn(sys::Service *sender, bool isOn) -> bool
    {
        auto msg = std::make_shared<app::manager::AutomaticDateAndTimeIsOnChangeRequest>(isOn);
        return sys::Bus::SendUnicast(std::move(msg), ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeAutomaticTimeZoneIsOn(sys::Service *sender, bool isOn) -> bool
    {
        auto msg = std::make_shared<app::manager::AutomaticTimeZoneIsOnChangeRequest>(isOn);
        return sys::Bus::SendUnicast(std::move(msg), ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeTimeFormat(sys::Service *sender, utils::time::Locale::TimeFormat timeFormat) -> bool
    {
        auto msg = std::make_shared<app::manager::TimeFormatChangeRequest>(timeFormat);
        return sys::Bus::SendUnicast(std::move(msg), ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeDateFormat(sys::Service *sender, utils::time::Locale::DateFormat dateFormat) -> bool
    {
        auto msg = std::make_shared<app::manager::DateFormatChangeRequest>(dateFormat);
        return sys::Bus::SendUnicast(std::move(msg), ApplicationManager::ServiceName, sender);
    }

    auto Controller::preventBlockingDevice(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::PreventBlockingRequest>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::changePowerSaveMode(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::PowerSaveModeInitRequest>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }
} // namespace app::manager
