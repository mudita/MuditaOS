// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Controller.hpp"

#include "FinishRequest.hpp"
#include "GetAllNotificationsRequest.hpp"

#include <Service/Service.hpp>
#include <service-appmgr/Constants.hpp>

#include <utility>

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
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::finish(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::FinishRequest>(sender->GetName());
        return sender->bus.sendUnicast(std::move(msg), service::name::appmgr);
    }

    auto Controller::sendAction(sys::Service *sender,
                                actions::ActionId actionId,
                                actions::ActionParamsPtr &&data,
                                OnSwitchBehaviour onSwitchBehaviour) -> bool
    {
        setOnSwitchBehaviour(data, onSwitchBehaviour);
        auto msg = std::make_shared<app::manager::ActionRequest>(sender->GetName(), actionId, std::move(data));
        return sender->bus.sendUnicast(std::move(msg), service::name::appmgr);
    }

    auto Controller::switchBack(sys::Service *sender, std::unique_ptr<SwitchBackRequest> msg) -> bool
    {
        std::shared_ptr<SwitchBackRequest> switchMsg =
            msg ? std::move(msg) : std::make_shared<app::manager::SwitchBackRequest>(sender->GetName());
        return sender->bus.sendUnicast(switchMsg, service::name::appmgr);
    }

    auto Controller::confirmSwitch(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::SwitchConfirmation>(sender->GetName());
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::closeApplication(sys::Service *sender, const ApplicationName &name) -> bool
    {
        auto msg = std::make_shared<app::manager::ApplicationCloseRequest>(sender->GetName(), name);
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::confirmClose(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::CloseConfirmation>(sender->GetName());
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::finalizingClose(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::FinalizingClose>(sender->GetName());
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::changeDisplayLanguage(sys::Service *sender, const Language &language) -> bool
    {
        auto msg = std::make_shared<app::manager::DisplayLanguageChangeRequest>(sender->GetName(), language);
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::changeInputLanguage(sys::Service *sender, const Language &language) -> bool
    {
        auto msg = std::make_shared<app::manager::InputLanguageChangeRequest>(sender->GetName(), language);
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::preventBlockingDevice(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::PreventBlockingRequest>(sender->GetName());
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::changePowerSaveMode(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::PowerSaveModeInitRequest>(sender->GetName());
        return sender->bus.sendUnicast(msg, service::name::appmgr);
    }

    auto Controller::requestNotifications(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::GetAllNotificationsRequest>();
        return sender->bus.sendUnicast(std::move(msg), service::name::appmgr);
    }
} // namespace app::manager
