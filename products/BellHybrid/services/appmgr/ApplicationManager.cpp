// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <appmgr/ApplicationManager.hpp>
#include <appmgr/messages/AlarmMessage.hpp>
#include <appmgr/messages/IdleTimerMessage.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutMessage.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <application-bell-onboarding/BellOnBoardingNames.hpp>
#include <application-bell-whats-new/ApplicationWhatsNew.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-desktop/Constants.hpp>
#include <popups/ChargingNotificationPopupRequestParams.hpp>
#include <popups/ChargingDoneNotificationPopupRequestParams.hpp>
#include <product/version.hpp>
#include <module-utils/utility/Version.hpp>

namespace app::manager
{
    ApplicationManager::ApplicationManager(const ApplicationName &serviceName,
                                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                                           const ApplicationName &_rootApplicationName)
        : ApplicationManagerCommon(serviceName, std::move(launchers), _rootApplicationName), IdleHandler(this)
    {
        registerMessageHandlers();
    }

    auto ApplicationManager::startApplication(ApplicationHandle &app) -> bool
    {
        if (not ApplicationManagerCommon::startApplication(app)) {
            LOG_INFO("Starting application %s", app.name().c_str());
            app.run(StatusIndicators{}, this);
        }
        return true;
    }

    auto ApplicationManager::resolveHomeApplication() -> std::string
    {
        if (checkOnBoarding()) {
            return app::applicationBellOnBoardingName;
        }
        else if (isWhatsNewAvailable()) {
            return app::applicationWhatsNewName;
        }
        return rootApplicationName;
    }

    ActionProcessStatus ApplicationManager::handleAction(ActionEntry &action)
    {
        switch (action.actionId) {
        default:
            return ApplicationManagerCommon::handleAction(action);
        }
    }

    void ApplicationManager::handleStart(StartAllowedMessage *msg)
    {
        switch (msg->getStartupType()) {
        case StartupType::CriticalBattery:
            handleSwitchApplication(
                std::make_unique<SwitchRequest>(
                    service::name::appmgr, resolveHomeApplication(), gui::window::name::bell_battery_shutdown, nullptr)
                    .get());

            break;
        default:
            ApplicationManagerCommon::handleStart(msg);
        }
    }

    void ApplicationManager::registerMessageHandlers()
    {
        ApplicationManagerCommon::registerMessageHandlers();

        bus.channels.push_back(sys::BusChannel::AlarmNotifications);

        auto convertibleToActionHandler = [this](sys::Message *request) { return handleMessageAsAction(request); };
        connect(typeid(StartIdleTimerMessage), [this](sys::Message *request) {
            handleStartIdleTimer(request);
            return sys::msgHandled();
        });
        connect(typeid(RestartIdleTimerMessage), [this](sys::Message *request) {
            handleRestartIdleTimer(request);
            return sys::msgHandled();
        });
        connect(typeid(StopIdleTimerMessage), [this](sys::Message *request) {
            handleStopIdleTimer(request);
            return sys::msgHandled();
        });

        connect(typeid(sevm::BatteryChargingMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto *msg = dynamic_cast<sevm::BatteryChargingMessage *>(request);
            if (msg == nullptr) {
                return sys::msgNotHandled();
            }
            switch (msg->getChargingState()) {
            case BatteryState::ChargingState::Charging:
                app::manager::Controller::sendAction(this,
                                                     app::manager::actions::ShowPopup,
                                                     std::make_unique<gui::ChargingNotificationPopupRequestParams>());
                break;
            case BatteryState::ChargingState::ChargingDone:
                app::manager::Controller::sendAction(
                    this,
                    app::manager::actions::ShowPopup,
                    std::make_unique<gui::ChargingDoneNotificationPopupRequestParams>());
                break;
            default:
                break;
            }

            return sys::msgHandled();
        });

        connect(typeid(InformDateChanged), [this]([[maybe_unused]] sys::Message *request) {
            DBServiceAPI::InformDateChanged(this);
            return sys::msgHandled();
        });

        connect(typeid(AlarmActivated), convertibleToActionHandler);
        connect(typeid(AlarmDeactivated), convertibleToActionHandler);
        connect(typeid(BedtimeNotification), convertibleToActionHandler);
        connect(typeid(ChangeHomescreenLayoutMessage), convertibleToActionHandler);
    }

    auto ApplicationManager::isWhatsNewAvailable() -> bool
    {
        auto previousVersion =
            settings->getValue(settings::SystemProperties::osCurrentVersion, settings::SettingsScope::Global);
        if (previousVersion == utils::defaultPreviousVersion) {
            const auto version = utils::getPreviousVersionFromFile(purefs::dir::getTemporaryPath() /
                                                                   sdesktop::paths::recoveryStatusFilename);
            if (not version.has_value()) {
                return true; // If we cannot get the version, display all What's New
            }
            previousVersion = version.value();
        }
        return previousVersion != VERSION;
    }
} // namespace app::manager
