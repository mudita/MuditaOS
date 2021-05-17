// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "config.h"

// applications
#include <application-antenna/ApplicationAntenna.hpp>
#include <application-call/ApplicationCall.hpp>
#include <application-calllog/ApplicationCallLog.hpp>
#include <application-desktop/ApplicationDesktop.hpp>
#include <application-messages/ApplicationMessages.hpp>
#include <application-notes/ApplicationNotes.hpp>
#include <application-phonebook/ApplicationPhonebook.hpp>
#include <application-settings/ApplicationSettings.hpp>
#include <application-settings-new/ApplicationSettings.hpp>
#include <application-special-input/ApplicationSpecialInput.hpp>
#include <application-calendar/ApplicationCalendar.hpp>
#include <application-music-player/ApplicationMusicPlayer.hpp>
#include <application-meditation/ApplicationMeditation.hpp>
#include <application-calculator/ApplicationCalculator.hpp>
#include <application-alarm-clock/ApplicationAlarmClock.hpp>
#include <application-onboarding/ApplicationOnBoarding.hpp>

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

#if ENABLE_GSM == 1
#include <service-fota/ServiceFota.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-antenna/ServiceAntenna.hpp>
#endif

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
#if ENABLE_GSM == 0
    // For now disable permanently Service cellular when there is no GSM configured
    LOG_INFO("ServiceCellular (GSM) - Disabled");
#else
    systemServices.emplace_back(sys::CreatorFor<ServiceAntenna>());
    systemServices.emplace_back(sys::CreatorFor<ServiceCellular>());
    systemServices.emplace_back(sys::CreatorFor<FotaService::Service>());
#endif
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
#ifdef ENABLE_APP_DESKTOP
            applications.push_back(
                app::CreateLauncher<app::ApplicationDesktop>(app::name_desktop, app::Closeable::False));
#endif
#ifdef ENABLE_APP_CALL
            applications.push_back(app::CreateLauncher<app::ApplicationCall>(app::name_call, app::Closeable::False));
#endif
#ifdef ENABLE_APP_SETTINGS
            applications.push_back(app::CreateLauncher<app::ApplicationSettings>(app::name_settings));
#endif
#ifdef ENABLE_APP_SETTINGS_NEW
            applications.push_back(app::CreateLauncher<app::ApplicationSettingsNew>(app::name_settings_new));
#endif
#ifdef ENABLE_APP_NOTES
            applications.push_back(app::CreateLauncher<app::ApplicationNotes>(app::name_notes));
#endif
#ifdef ENABLE_APP_CALLLOG
            applications.push_back(app::CreateLauncher<app::ApplicationCallLog>(app::CallLogAppStr));
#endif
#ifdef ENABLE_APP_PHONEBOOK
            applications.push_back(app::CreateLauncher<app::ApplicationPhonebook>(app::name_phonebook));
#endif
#ifdef ENABLE_APP_MESSAGES
            applications.push_back(app::CreateLauncher<app::ApplicationMessages>(app::name_messages));
#endif
#ifdef ENABLE_APP_SPECIAL_INPUT
            applications.push_back(
                app::CreateLauncher<app::ApplicationSpecialInput>(app::special_input, app::Closeable::False));
#endif
#ifdef ENABLE_APP_ANTENNA
            applications.push_back(app::CreateLauncher<app::ApplicationAntenna>(app::name_antenna));
#endif
#ifdef ENABLE_APP_CALENDAR
            applications.push_back(app::CreateLauncher<app::ApplicationCalendar>(app::name_calendar));
#endif
#ifdef ENABLE_APP_MUSIC_PLAYER
            applications.push_back(app::CreateLauncher<app::ApplicationMusicPlayer>(app::name_music_player));
#endif
#ifdef ENABLE_APP_MEDITATION
            applications.push_back(
                app::CreateLauncher<app::ApplicationMeditation>(app::name_meditation, app::Closeable::True));
#endif
#ifdef ENABLE_APP_CALCULATOR
            applications.push_back(app::CreateLauncher<app::ApplicationCalculator>(app::name_calculator));
#endif
#ifdef ENABLE_APP_ALARM_CLOCK
            applications.push_back(app::CreateLauncher<app::ApplicationAlarmClock>(app::name_alarm_clock));
#endif
#ifdef ENABLE_APP_ONBOARDING
            applications.push_back(app::CreateLauncher<app::ApplicationOnBoarding>(app::name_onboarding));
#endif
            // start application manager
            return sysmgr->RunSystemService(
                std::make_shared<app::manager::ApplicationManager>(
                    app::manager::ApplicationManager::ServiceName, std::move(applications), app::name_desktop),
                sysmgr.get());
        });

    LOG_PRINTF("Launching PurePhone \n");
    LOG_PRINTF("commit: %s tag: %s branch: %s\n", GIT_REV, GIT_TAG, GIT_BRANCH);
    cpp_freertos::Thread::StartScheduler();

    purefs::subsystem::unmount_all();

    return 0;
}
