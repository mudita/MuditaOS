// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Controller.hpp"

#include "module-sys/Service/Bus.hpp"

namespace app::manager
{
    auto Controller::registerApplication(sys::Service *sender, bool status, bool startBackground) -> bool
    {
        auto msg = std::make_shared<app::manager::APMRegister>(sender->GetName(), status, startBackground);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::sendAction(sys::Service *sender, Action &&action) -> bool
    {
        auto msg = std::make_shared<app::manager::APMAction>(sender->GetName(), std::move(action));
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::switchApplication(sys::Service *sender,
                                       const ApplicationHandle::Name &applicationName,
                                       const std::string &windowName,
                                       std::unique_ptr<gui::SwitchData> data) -> bool
    {
        auto msg =
            std::make_shared<app::manager::APMSwitch>(sender->GetName(), applicationName, windowName, std::move(data));
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::confirmSwitch(sys::Service *sender) -> bool
    {

        auto msg = std::make_shared<app::manager::APMConfirmSwitch>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::confirmClose(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::APMConfirmClose>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::closeApplication(sys::Service *sender, const ApplicationHandle::Name &name) -> bool
    {
        auto msg = std::make_shared<app::manager::APMDelayedClose>(sender->GetName(), name);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::switchBack(sys::Service *sender, std::unique_ptr<APMSwitchPrevApp> msg) -> bool
    {

        std::shared_ptr<APMSwitchPrevApp> switchMsg =
            msg ? std::move(msg) : std::make_shared<app::manager::APMSwitchPrevApp>(sender->GetName());
        return sys::Bus::SendUnicast(switchMsg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::changeLanguage(sys::Service *sender, utils::Lang language) -> bool
    {
        auto msg = std::make_shared<app::manager::APMChangeLanguage>(sender->GetName(), language);
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::stopApplicationManager(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::APMClose>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::preventBlockingDevice(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::APMPreventBlocking>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }

    auto Controller::changePowerSaveMode(sys::Service *sender) -> bool
    {
        auto msg = std::make_shared<app::manager::APMInitPowerSaveMode>(sender->GetName());
        return sys::Bus::SendUnicast(msg, ApplicationManager::ServiceName, sender);
    }
} // namespace app::manager
