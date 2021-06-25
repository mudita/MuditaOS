// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "config.h"

// applications
#include <application-call/ApplicationCall.hpp>
#include <application-calllog/ApplicationCallLog.hpp>
#include <application-desktop/ApplicationDesktop.hpp>
#include <application-messages/ApplicationMessages.hpp>
#include <application-notes/ApplicationNotes.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <application-settings-new/ApplicationSettings.hpp>
#include <application-special-input/ApplicationSpecialInput.hpp>
#include <application-calendar/ApplicationCalendar.hpp>
#include <application-music-player/ApplicationMusicPlayer.hpp>
#include <application-meditation/ApplicationMeditation.hpp>
#include <application-calculator/ApplicationCalculator.hpp>
#include <application-alarm-clock/ApplicationAlarmClock.hpp>
#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <application-bell-main/ApplicationBellMain.hpp>

// services
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-audio/ServiceAudio.hpp>
#include <service-bluetooth/ServiceBluetooth.hpp>
#include <service-db/ServiceDB.hpp>
#include <service-evtmgr/EventManager.hpp>
#include <service-lwip/ServiceLwIP.hpp>
#include <service-time/ServiceTime.hpp>
#include <Service/ServiceCreator.hpp>
#include <service-gui/ServiceGUI.hpp>
#include <service-gui/Common.hpp>
#include <module-services/service-eink/ServiceEink.hpp>
#include <service-fileindexer/Constants.hpp>
#include <service-fileindexer/ServiceFileIndexer.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <bsp/bsp.hpp>
#include <Application.hpp>
#include <ApplicationLauncher.hpp>
#include <log/log.hpp>
#include <log/Logger.hpp>
#include <phonenumbers/phonenumberutil.h>
#include <source/version.hpp>
#include <SystemManager/SystemManager.hpp>
#include <SystemWatchdog/SystemWatchdog.hpp>
#include <thread.hpp>
#include <purefs/vfs_subsystem.hpp>

#include <memory>
#include <vector>

int main()
{

#if SYSTEM_VIEW_ENABLED
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_ENTER);
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_EXIT);
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_TO_SCHEDULER);
    SEGGER_SYSVIEW_WaitForConnection();
    SEGGER_SYSVIEW_Start();
#endif

    bsp::BoardInit();

    purefs::subsystem::vfs_handle_t vfs;
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
    systemServices.emplace_back(sys::CreatorFor<ServiceBluetooth>());
    systemServices.emplace_back(sys::CreatorFor<ServiceLwIP>());
    systemServices.emplace_back(sys::CreatorFor<ServiceDesktop>());
    systemServices.emplace_back(sys::CreatorFor<stm::ServiceTime>());
    systemServices.emplace_back(sys::CreatorFor<service::eink::ServiceEink>());
    systemServices.emplace_back(sys::CreatorFor<service::gui::ServiceGUI>());

    auto sysmgr = std::make_shared<sys::SystemManager>(std::move(systemServices));
    sysmgr->StartSystem(
        [&vfs]() {
            vfs     = purefs::subsystem::initialize();
            int err = purefs::subsystem::mount_defaults();
            if (err) {
                LOG_FATAL("VFS subystem fatal error %i", err);
                std::abort();
            }
            Log::Logger::get().init();
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
            // start application manager
            return sysmgr->RunSystemService(
                std::make_shared<app::manager::ApplicationManager>(
                    app::manager::ApplicationManager::ServiceName, std::move(applications), app::applicationBellName),
                sysmgr.get());
        });

    LOG_PRINTF("Launching BellHybrid \n");
    LOG_PRINTF("commit: %s tag: %s branch: %s\n", GIT_REV, GIT_TAG, GIT_BRANCH);
    cpp_freertos::Thread::StartScheduler();

    purefs::subsystem::unmount_all();

    return 0;
}
