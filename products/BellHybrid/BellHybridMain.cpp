﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PlatformFactory.hpp"

// applications
#include <application-bell-alarm/ApplicationBellAlarm.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>
#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <application-bell-powernap/ApplicationBellPowerNap.hpp>

// modules
#include <module-db/Databases/AlarmsDB.hpp>
#include <module-db/Databases/EventsDB.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/AlarmsRecord.hpp>

// services
#include <appmgr/ApplicationManager.hpp>
#include <db/ServiceDB.hpp>
#include <evtmgr/EventManager.hpp>
#include <module-services/service-eink/ServiceEink.hpp>
#include <Service/ServiceCreator.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-audio/ServiceAudio.hpp>
#include <service-bluetooth/ServiceBluetooth.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <service-fileindexer/Constants.hpp>
#include <service-gui/ServiceGUI.hpp>
#include <service-lwip/ServiceLwIP.hpp>
#include <service-time/ServiceTime.hpp>

#include <Application.hpp>
#include <ApplicationLauncher.hpp>
#include <log/Logger.hpp>
#include <log/log.hpp>
#include <source/version.hpp>
#include <sys/SystemManager.hpp>
#include <SystemWatchdog/SystemWatchdog.hpp>
#include <thread.hpp>

#include <memory>
#include <vector>

int main()
{
    constexpr auto ApplicationName = "BellHybrid";

#if SYSTEM_VIEW_ENABLED
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_ENTER);
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_EXIT);
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_TO_SCHEDULER);
    SEGGER_SYSVIEW_WaitForConnection();
    SEGGER_SYSVIEW_Start();
#endif

    auto platformFactory = bellhybrid::PlatformFactory();
    auto platform        = platformFactory.makePlatform();

    if (!sys::SystemWatchdog::getInstance().init()) {
        LOG_ERROR("System watchdog failed to initialize");
        // wait for the hardware watchdog (initialized in reset ISR) to reset the system
        while (1)
            ;
    }

    std::vector<std::unique_ptr<sys::BaseServiceCreator>> systemServices;
    systemServices.emplace_back(sys::CreatorFor<EventManager>());
#if ENABLE_FILEINDEXER_SERVICE
    systemServices.emplace_back(sys::CreatorFor<service::ServiceFileIndexer>());
#endif
    systemServices.emplace_back(sys::CreatorFor<ServiceDB>());
    systemServices.emplace_back(sys::CreatorFor<ServiceAudio>());
    systemServices.emplace_back(sys::CreatorFor<ServiceDesktop>());
    systemServices.emplace_back(sys::CreatorFor<stm::ServiceTime>());
    systemServices.emplace_back(sys::CreatorFor<service::eink::ServiceEink>());
    systemServices.emplace_back(sys::CreatorFor<service::gui::ServiceGUI>());

    auto sysmgr = std::make_shared<sys::SystemManager>(std::move(systemServices));
    sysmgr->StartSystem(
        [&platform]() {
            try {
                platform->init();
            }
            catch (const std::runtime_error &e) {
                LOG_FATAL("%s", e.what());
                abort();
            }

            Log::Logger::get().init(Log::Application{ApplicationName, GIT_REV, GIT_TAG, GIT_BRANCH});
            /// force initialization of PhonenumberUtil because of its stack usage
            /// otherwise we would end up with an init race and PhonenumberUtil could
            /// be initiated in a task with stack not big enough to handle it
            i18n::phonenumbers::PhoneNumberUtil::GetInstance();
            return true;
        },
        [sysmgr]() {
            // vector with launchers to applications
            std::vector<std::unique_ptr<app::ApplicationLauncher>> applications;
            applications.push_back(
                app::CreateLauncher<app::ApplicationBellMain>(app::applicationBellName, app::Closeable::False));
            applications.push_back(app::CreateLauncher<app::ApplicationBellSettings>(app::applicationBellSettingsName));
            applications.push_back(app::CreateLauncher<app::ApplicationBellAlarm>(app::applicationBellAlarmName));
            applications.push_back(app::CreateLauncher<app::ApplicationBellPowerNap>(app::applicationBellPowerNapName));
            // start application manager
            return sysmgr->RunSystemService(
                std::make_shared<app::manager::ApplicationManager>(
                    service::name::appmgr, std::move(applications), app::applicationBellName),
                sysmgr.get());
        });

    LOG_PRINTF("Launching %s \n", ApplicationName);
    LOG_PRINTF("commit: %s tag: %s branch: %s\n", GIT_REV, GIT_TAG, GIT_BRANCH);
    cpp_freertos::Thread::StartScheduler();

    return 0;
}
