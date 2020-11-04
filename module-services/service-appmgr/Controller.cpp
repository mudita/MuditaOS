// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/Controller.hpp"

#include "service-appmgr/model/ApplicationManager.hpp"

#include <module-sys/Service/Bus.hpp>
#include <Service/Service.hpp>

#include <utility> // for move
namespace app::manager
{
    auto Controller::sendAction(sys::Service *sender, actions::ActionId actionId, actions::ActionParamsPtr &&data)
        -> bool
    {
        auto msg = std::make_shared<app::manager::ActionRequest>(sender->GetName(), actionId, std::move(data));
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::applicationInitialised(sys::Service *sender,
                                            StartupStatus status,
                                            StartInBackground startInBackground) -> bool
    {
        auto msg =
            std::make_shared<app::manager::ApplicationInitialisation>(sender->GetName(), status, startInBackground);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::switchApplication(sys::Service *sender,
                                       const ApplicationName &applicationName,
                                       const std::string &windowName,
                                       std::unique_ptr<gui::SwitchData> data) -> bool
    {
        auto msg = std::make_shared<app::manager::SwitchRequest>(
            sender->GetName(), applicationName, windowName, std::move(data));
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::confirmSwitch(sys::Service *sender) -> bool
    {

        auto msg = std::make_shared<app::manager::SwitchConfirmation>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::confirmClose(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::CloseConfirmation>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::closeApplication(sys::Service *sender, const ApplicationName &name) -> bool
    {
        auto msg = std::make_shared<app::manager::ApplicationCloseRequest>(sender->GetName(), name);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::switchBack(sys::Service *sender, std::unique_ptr<SwitchBackRequest> msg) -> bool
    {

        std::shared_ptr<SwitchBackRequest> switchMsg =
            msg ? std::move(msg) : std::make_shared<app::manager::SwitchBackRequest>(sender->GetName());
        return sys::Bus::SendUnicast(switchMsg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeLanguage(sys::Service *sender, utils::Lang language) -> bool
    {
        auto msg = std::make_shared<app::manager::LanguageChangeRequest>(sender->GetName(), language);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::stopApplicationManager(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::ShutdownRequest>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
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
