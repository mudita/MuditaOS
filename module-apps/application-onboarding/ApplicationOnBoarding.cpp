// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <utility>

#include "ApplicationOnBoarding.hpp"

#include "windows/OnBoardingMainWindow.hpp"
#include "windows/StartConfigurationWindow.hpp"
#include "windows/OnBoardingLanguagesWindow.hpp"
#include "windows/EULALicenseWindow.hpp"
#include "windows/ConfigurationSuccessfulDialogWindow.hpp"
#include "windows/NoConfigurationDialogWindow.hpp"
#include "windows/UpdateDialogWindow.hpp"
#include "windows/SkipDialogWindow.hpp"

#include <service-db/DBMessage.hpp>
#include <module-services/service-appmgr/service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>
#include <module-apps/application-settings-new/data/LanguagesData.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <module-apps/windows/Dialog.hpp>

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
                                         Indicator::NetworkAccessTechnology,
                                         Indicator::PhoneMode});

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

        return sys::msgNotHandled();
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
                switchWindow(gui::window::name::onBoarding_eula, nullptr);
                return sys::msgHandled();
            }
            else {
                return sys::msgNotHandled();
            }
        });

        return ret;
    }

    void ApplicationOnBoarding::acceptEULA()
    {
        settings->setValue(settings::SystemProperties::eulaAccepted, "1", settings::SettingsScope::Global);
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
            return std::make_unique<app::onBoarding::OnBoardingMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::onBoarding_languages, [](Application *app, const std::string &name) {
            return std::make_unique<app::onBoarding::OnBoardingLanguagesWindow>(app);
        });
        windowsFactory.attach(gui::window::name::onBoarding_start_configuration,
                              [](Application *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::StartConfigurationWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_eula, [&](Application *app, const std::string &name) {
            auto eulaRepository = std::make_unique<app::onBoarding::EULARepository>("assets/licenses", "eula.txt");
            auto presenter      = std::make_unique<app::onBoarding::EULALicenseWindowPresenter>([&]() { acceptEULA(); },
                                                                                           std::move(eulaRepository));
            return std::make_unique<app::onBoarding::EULALicenseWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::window::name::onBoarding_configuration_successful,
                              [](Application *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::ConfigurationSuccessfulDialogWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_no_configuration,
                              [](Application *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::NoConfigurationDialogWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_update, [](Application *app, const std::string &name) {
            return std::make_unique<app::onBoarding::UpdateDialogWindow>(app);
        });

        windowsFactory.attach(gui::window::name::onBoarding_skip, [](Application *app, const std::string &name) {
            return std::make_unique<app::onBoarding::SkipDialogWindow>(app);
        });

        attachPopups({gui::popup::ID::Volume, gui::popup::ID::PhoneModes});
    }

    void ApplicationOnBoarding::destroyUserInterface()
    {}

} // namespace app
