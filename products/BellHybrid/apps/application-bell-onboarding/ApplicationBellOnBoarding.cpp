// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellOnBoarding.hpp"

#include <presenter/OnBoardingLanguageWindowPresenter.hpp>
#include <windows/OnBoardingLanguageWindow.hpp>
#include <windows/OnBoardingFinalizeWindow.hpp>
#include <windows/OnBoardingSettingsWindow.hpp>

#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>

#include <apps-common/messages/OnBoardingMessages.hpp>
#include <common/BellFinishedWindow.hpp>

#include <application-bell-settings/models/TemperatureUnitModel.hpp>
#include <application-bell-settings/models/TimeUnitsModel.hpp>
#include <application-bell-settings/presenter/TimeUnitsPresenter.hpp>

namespace app
{
    ApplicationBellOnBoarding::ApplicationBellOnBoarding(std::string name,
                                                         std::string parent,
                                                         StatusIndicators statusIndicators,
                                                         StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellOnBoarding::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        connect(typeid(manager::GetCurrentDisplayLanguageResponse), [&](sys::Message *msg) {
            if (gui::name::window::main_window == getCurrentWindow()->getName()) {
                switchWindow(gui::window::name::onBoardingSettingsWindow);
                return sys::msgHandled();
            }
            return sys::msgNotHandled();
        });

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellOnBoarding::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<OnBoarding::OnBoardingLanguageWindowPresenter>(this);
            return std::make_unique<gui::OnBoardingLanguageWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(
            gui::window::name::onBoardingSettingsWindow, [](ApplicationCommon *app, const std::string &name) {
                auto temperatureUnitModel = std::make_unique<bell_settings::TemperatureUnitModel>(app);
                auto timeUnitsProvider    = std::make_shared<bell_settings::TimeUnitsModel>(app);
                auto presenter            = std::make_unique<bell_settings::TimeUnitsWindowPresenter>(
                    timeUnitsProvider, std::move(temperatureUnitModel));
                return std::make_unique<gui::OnBoardingSettingsWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(
            gui::window::name::finalizeOnBoardingWindow, [this](ApplicationCommon *app, const std::string &name) {
                auto presenter =
                    std::make_unique<OnBoarding::OnBoardingFinalizeWindowPresenter>([&]() { finalizeOnBoarding(); });
                return std::make_unique<gui::OnBoardingFinalizeWindow>(app, std::move(presenter));
            });
    }

    void ApplicationBellOnBoarding::destroyUserInterface()
    {}

    sys::ReturnCodes ApplicationBellOnBoarding::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }

    sys::MessagePointer ApplicationBellOnBoarding::DataReceivedHandler(sys::DataMessage *msgl,
                                                                       sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    void ApplicationBellOnBoarding::finalizeOnBoarding()
    {
        bus.sendUnicast(std::make_shared<onBoarding::FinalizeOnBoarding>(), service::name::appmgr);
    }
} // namespace app
