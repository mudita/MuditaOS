// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <appmgr/ApplicationManager.hpp>
#include <appmgr/messages/AlarmMessage.hpp>
#include <appmgr/messages/IdleTimerMessage.hpp>
#include <appmgr/messages/ChangeHomescreenLayoutMessage.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <application-bell-onboarding/BellOnBoardingNames.hpp>
#include <service-appmgr/Constants.hpp>
#include <common/windows/BellWelcomeWindow.hpp>
#include <service-appmgr/Controller.hpp>

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
        return rootApplicationName;
    }

    ActionProcessStatus ApplicationManager::handleAction(ActionEntry &action)
    {
        switch (action.actionId) {
        case actions::FileTransferStarted:
            return handleFileTransferStarted(action);
        case actions::FileTransferFinished:
            return ApplicationManagerCommon::handleActionOnFocusedApp(action);
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

    auto ApplicationManager::handleDisplayLanguageChange(app::manager::DisplayLanguageChangeRequest *msg) -> bool
    {
        return ApplicationManagerCommon::handleDisplayLanguageChange(msg);
    }

    void ApplicationManager::registerMessageHandlers()
    {
        ApplicationManagerCommon::registerMessageHandlers();

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
        connect(typeid(AlarmActivated), convertibleToActionHandler);
        connect(typeid(AlarmDeactivated), convertibleToActionHandler);
        connect(typeid(BedtimeNotification), convertibleToActionHandler);
        connect(typeid(ChangeHomescreenLayoutMessage), convertibleToActionHandler);
        //        connect(typeid(sdesktop::fileTransfer::FileTransferMessage), [this](sys::Message *request) {
        //            handleFileTransferMessage(request);
        //            return sys::msgHandled();
        //        });
    }
    auto ApplicationManager::handleFileTransferStarted(ActionEntry &action) -> ActionProcessStatus
    {
        LOG_ERROR("XXX handleFileTransferStarted: ");
        const auto appName = "ApplicationBellFileTransfer";
        action.setTargetApplication(appName);

        SwitchRequest switchRequest(service::name::appmgr, appName, resolveHomeWindow(), std::move(action.params));
        return handleSwitchApplication(&switchRequest) ? ActionProcessStatus::Accepted : ActionProcessStatus::Dropped;
        //        return ActionProcessStatus::Accepted;
    }
    auto ApplicationManager::handleFileTransferFinished(ActionEntry &action) -> ActionProcessStatus
    {
        //        LOG_ERROR("XXX handleFileTransferFinished: ");
        //        //        const auto appName = "ApplicationBellFileTransfer";
        //        action.setTargetApplication(rootApplicationName);
        //
        //        SwitchRequest switchRequest(
        //            service::name::appmgr, rootApplicationName, resolveHomeWindow(), std::move(action.params));
        //        return handleSwitchApplication(&switchRequest) ? ActionProcessStatus::Accepted :
        //        ActionProcessStatus::Dropped;
        return ActionProcessStatus::Accepted;
    }
} // namespace app::manager
