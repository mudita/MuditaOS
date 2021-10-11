// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <appmgr/ApplicationManager.hpp>
#include <appmgr/messages/AlarmMessage.hpp>
#include <appmgr/messages/IdleTimerMessage.hpp>

#include <application-bell-onboarding/BellOnBoardingNames.hpp>

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
    }
} // namespace app::manager
