// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationOnBoarding.hpp"

#include "apps-common/messages/OnBoardingMessages.hpp"
#include "windows/StartConfigurationWindow.hpp"
#include "windows/OnBoardingLanguagesWindow.hpp"
#include "windows/EULALicenseWindow.hpp"
#include "windows/OnBoardingSimSelectWindow.hpp"
#include "windows/NoSimSelectedDialogWindow.hpp"
#include "windows/ConfigurationSuccessfulDialogWindow.hpp"
#include "windows/NoConfigurationDialogWindow.hpp"
#include "windows/UpdateDialogWindow.hpp"
#include "windows/SkipDialogWindow.hpp"
#include "windows/OnBoardingDateAndTimeWindow.hpp"
#include "windows/OnBoardingChangeDateAndTimeWindow.hpp"
#include "WindowsPopupFilter.hpp"

#include <application-settings/windows/system/ChangeTimeZone.hpp>
#include <apps-common/locks/data/PhoneLockMessages.hpp>
#include <apps-common/locks/data/SimLockMessages.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <EventStore.hpp>

namespace app
{
    namespace
    {
        constexpr auto OnBoardingStackSize = 4096U;
    } // namespace

    ApplicationOnBoarding::ApplicationOnBoarding(std::string name,
                                                 std::string parent,
                                                 StatusIndicators statusIndicators,
                                                 StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, OnBoardingStackSize)
    {
        using namespace gui::status_bar;
        statusBarManager->enableIndicators(
            {Indicator::Signal, Indicator::Time, Indicator::Battery, Indicator::SimCard});

        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        bus.channels.push_back(sys::BusChannel::PhoneLockChanges);
        bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);

        getPopupFilter().addAppDependentFilter([&](const gui::PopupRequestParams & /*popupParams*/) {
            return gui::name::window::main_window != getCurrentWindow()->getName();
        });
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationOnBoarding::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }

    sys::ReturnCodes ApplicationOnBoarding::InitHandler()
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        settings->setValue(
            settings::SystemProperties::eulaAccepted, utils::to_string(false), settings::SettingsScope::Global);

        connect(typeid(manager::GetCurrentDisplayLanguageResponse), [&](sys::Message *msg) {
            if (gui::name::window::main_window == getCurrentWindow()->getName()) {
                switchWindow(gui::window::name::onBoarding_eula, nullptr);
                return sys::msgHandled();
            }
            else {
                return sys::msgNotHandled();
            }
        });

        connect(typeid(locks::SetConfirmedPhoneLock), [&](sys::Message *msg) {
            switchWindow(gui::window::name::onBoarding_date_and_time);
            return sys::msgHandled();
        });

        connect(typeid(locks::SkippedSetPhoneLock), [&](sys::Message *msg) {
            switchWindow(gui::window::name::onBoarding_date_and_time);
            return sys::msgHandled();
        });

        connect(typeid(locks::SimSwitched), [&](sys::Message *msg) {
            auto simState = Store::GSM::get()->sim;
            if (simState == Store::GSM::SIM::SIM1 || simState == Store::GSM::SIM::SIM2) {
                phoneLockSubject.setPhoneLock();
                return sys::msgHandled();
            }
            return sys::msgNotHandled();
        });

        connect(typeid(cellular::msg::notification::SimReady), [&](sys::Message *msg) {
            if (getCurrentWindow()->getName() != gui::popup::window::sim_switching_window) {
                phoneLockSubject.setPhoneLock();
                return sys::msgHandled();
            }
            return sys::msgNotHandled();
        });

        return ret;
    }

    void ApplicationOnBoarding::acceptEULA()
    {
        settings->setValue(
            settings::SystemProperties::eulaAccepted, utils::to_string(true), settings::SettingsScope::Global);
    }

    void ApplicationOnBoarding::finalizeOnBoarding()
    {
        bus.sendUnicast(std::make_shared<onBoarding::FinalizeOnBoarding>(), service::name::appmgr);
    }

    sys::ReturnCodes ApplicationOnBoarding::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationOnBoarding::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<app::onBoarding::OnBoardingLanguagesWindow>(app, gui::name::window::main_window);
        });
        windowsFactory.attach(gui::window::name::onBoarding_start_configuration,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::StartConfigurationWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_eula, [&](ApplicationCommon *app, const std::string &name) {
            const auto eulaDirPath  = purefs::dir::getDataDirPath() / "licenses";
            const auto eulaFilename = "eula.txt";
            auto eulaRepository     = std::make_unique<app::onBoarding::EULARepository>(eulaDirPath, eulaFilename);
            auto presenter      = std::make_unique<app::onBoarding::EULALicenseWindowPresenter>([&]() { acceptEULA(); },
                                                                                           std::move(eulaRepository));
            return std::make_unique<app::onBoarding::EULALicenseWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::window::name::onBoarding_sim_select,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::OnBoardingSimSelectWindow>(
                                      app, gui::window::name::onBoarding_sim_select);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_no_sim_selected,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::NoSimSelectedDialogWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_configuration_successful,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::ConfigurationSuccessfulDialogWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_no_configuration,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::NoConfigurationDialogWindow>(app);
                              });
        windowsFactory.attach(
            gui::window::name::onBoarding_update, [&](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<app::onBoarding::OnBoardingFinalizeWindowPresenter>(
                    [&]() { finalizeOnBoarding(); });
                return std::make_unique<app::onBoarding::UpdateDialogWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(gui::window::name::onBoarding_skip, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<app::onBoarding::SkipDialogWindow>(app);
        });
        windowsFactory.attach(gui::window::name::onBoarding_date_and_time,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::OnBoardingDateAndTimeWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::onBoarding_change_date_and_time,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<app::onBoarding::OnBoardingChangeDateAndTimeWindow>(app);
                              });
        windowsFactory.attach(gui::window::name::change_time_zone, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::ChangeTimeZone>(app);
        });
        windowsFactory.attach(gui::window::name::dialog_confirm, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, gui::window::name::dialog_confirm);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::SimLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationOnBoarding::destroyUserInterface()
    {}

} // namespace app
