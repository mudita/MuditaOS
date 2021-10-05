// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <appmgr/ApplicationManager.hpp>
#include <appmgr/messages/AlarmMessage.hpp>
#include <appmgr/messages/IdleTimerMessage.hpp>
#include <application-bell-onboarding/BellOnBoardingNames.hpp>

#include <Timers/TimerFactory.hpp>

namespace app::manager
{
    ApplicationManager::ApplicationManager(const ApplicationName &serviceName,
                                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                                           const ApplicationName &_rootApplicationName)
        : ApplicationManagerCommon(serviceName, std::move(launchers), _rootApplicationName)
    {
        registerMessageHandlers();
        idleTimer = sys::TimerFactory::createPeriodicTimer(this,
                                                           "IdleReturn",
                                                           idleReturnTimeout,
                                                           [this](sys::Timer &) { idleTimerCallback(); });
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

    void ApplicationManager::handleIdleTimerRestart(sys::Message *request)
    {
        idleTimer.restart(idleReturnTimeout);
    }

    void ApplicationManager::handleIdleTimerStop(sys::Message *request)
    {
        idleTimer.stop();
    }

    void ApplicationManager::idleTimerCallback()
    {
        LOG_INFO("Idle timer rang - going back to home screen");
        idleTimer.stop();
        ActionEntry entry{app::manager::actions::Home, {}};
        handleAction(entry);
    }

    void ApplicationManager::registerMessageHandlers()
    {
        ApplicationManagerCommon::registerMessageHandlers();

        auto convertibleToActionHandler = [this](sys::Message *request) { return handleMessageAsAction(request); };
        connect(typeid(IdleTimerRestart), [this](sys::Message *request) {
            handleIdleTimerRestart(request);
            return sys::msgHandled();
        });
        connect(typeid(IdleTimerStop), [this](sys::Message *request) {
            handleIdleTimerStop(request);
            return sys::msgHandled();
        });
        connect(typeid(AlarmActivated), convertibleToActionHandler);
        connect(typeid(AlarmDeactivated), convertibleToActionHandler);
    }
} // namespace app::manager
