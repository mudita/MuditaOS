// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <utility>

#include "ApplicationOnBoarding.hpp"

#include "windows/OnBoardingMainWindow.hpp"
#include "windows/StartConfigurationWindow.hpp"
#include "windows/OnBoardingLanguagesWindow.hpp"

#include <service-db/DBMessage.hpp>
#include <module-services/service-appmgr/service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>
#include <module-apps/application-settings-new/data/LanguagesData.hpp>

namespace app
{
    namespace
    {
        constexpr auto OnBoardingStackSize = 4096U;
    } // namespace

    ApplicationOnBoarding::ApplicationOnBoarding(std::string name,
                                                 std::string parent,
                                                 StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), startInBackground, OnBoardingStackSize)
    {
        using namespace gui::top_bar;
        topBarManager->enableIndicators({Indicator::Signal,
                                         Indicator::Time,
                                         Indicator::Battery,
                                         Indicator::SimCard,
                                         Indicator::NetworkAccessTechnology});

        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationOnBoarding::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return msgNotHandled();
    }

    sys::ReturnCodes ApplicationOnBoarding::InitHandler()
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);

        connect(typeid(manager::GetCurrentDisplayLanguageResponse), [&](sys::Message *msg) {
            if (gui::window::name::onBoarding_languages == getCurrentWindow()->getName()) {
                switchWindow(gui::window::name::onBoarding_start_configuration, nullptr);
                return msgHandled();
            }
            else {
                return msgNotHandled();
            }
        });

        return ret;
    }

    sys::ReturnCodes ApplicationOnBoarding::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ApplicationOnBoarding::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationOnBoarding::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::OnBoardingMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::onBoarding_languages, [](Application *app, const std::string &name) {
            return std::make_unique<gui::OnBoardingLanguagesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::onBoarding_start_configuration,
                              [](Application *app, const std::string &name) {
                                  return std::make_unique<gui::StartConfigurationWindow>(app);
                              });
    }

    void ApplicationOnBoarding::destroyUserInterface()
    {}
} // namespace app
