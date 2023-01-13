// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "config.h"
#include "PlatformFactory.hpp"

// applications
#ifdef ENABLE_APP_ANTENNA
#include <application-antenna/ApplicationAntenna.hpp>
#endif
#ifdef ENABLE_APP_ALARM_CLOCK
#include <application-alarm-clock/ApplicationAlarmClock.hpp>
#endif
#ifdef ENABLE_APP_CALL
#include <application-call/ApplicationCall.hpp>
#endif
#ifdef ENABLE_APP_CALLLOG
#include <application-calllog/ApplicationCallLog.hpp>
#endif
#ifdef ENABLE_APP_DESKTOP
#include <application-desktop/ApplicationDesktop.hpp>
#endif
#ifdef ENABLE_APP_MESSAGES
#include <application-messages/ApplicationMessages.hpp>
#endif
#ifdef ENABLE_APP_NOTES
#include <application-notes/ApplicationNotes.hpp>
#endif
#ifdef ENABLE_APP_PHONEBOOK
#include <application-phonebook/ApplicationPhonebook.hpp>
#endif
#ifdef ENABLE_APP_SETTINGS
#include <application-settings/ApplicationSettings.hpp>
#endif
#ifdef ENABLE_APP_SPECIAL_INPUT
#include <application-special-input/ApplicationSpecialInput.hpp>
#endif
#ifdef ENABLE_APP_CALENDAR
#include <application-calendar/ApplicationCalendar.hpp>
#endif
#ifdef ENABLE_APP_MUSIC_PLAYER
#include <application-music-player/ApplicationMusicPlayer.hpp>
#endif
#ifdef ENABLE_APP_MEDITATION
#include <application-meditation/ApplicationMeditation.hpp>
#endif
#ifdef ENABLE_APP_CALCULATOR
#include <application-calculator/ApplicationCalculator.hpp>
#endif
#ifdef ENABLE_APP_ONBOARDING
#include <application-onboarding/ApplicationOnBoarding.hpp>
#endif
#ifdef ENABLE_APP_TEST
#include <application-test/ApplicationTest.hpp>
#endif

// modules
#include "module-db/databases/NotificationsDB.hpp"
#include <module-db/databases/MultimediaFilesDB.hpp>
#include <module-db/Interface/NotesRecord.hpp>
#include <module-db/Interface/NotificationsRecord.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>

// services
#include <appmgr/ApplicationManager.hpp>
#include <db/ServiceDB.hpp>
#include <evtmgr/EventManager.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-audio/ServiceAudio.hpp>
#include <service-bluetooth/ServiceBluetooth.hpp>
#include <service-db/agents/quotes/QuotesAgent.hpp>
#include <service-time/ServiceTime.hpp>
#include <Service/ServiceCreator.hpp>
#include <service-gui/ServiceGUI.hpp>
#include <service-gui/Common.hpp>
#include <module-services/service-eink/ServiceEink.hpp>
#if ENABLE_SERVICE_FILEINDEXER
#include <service-fileindexer/ServiceFileIndexer.hpp>
#endif
#include <service-desktop/ServiceDesktop.hpp>

#if ENABLE_GSM == 1
#include <service-cellular/ServiceCellular.hpp>
#include <service-antenna/ServiceAntenna.hpp>
#endif

#if ENABLE_SERVICE_TEST
#include <service-test/ServiceTest.hpp>
#endif

#include <Application.hpp>
#include <ApplicationLauncher.hpp>
#include <log/log.hpp>
#include <logdump/logdump.h>
#include <Logger.hpp>
#include <product/version.hpp>
#include <sys/SystemManager.hpp>
#include <SystemWatchdog/SystemWatchdog.hpp>
#include <thread.hpp>
#include <time/AlarmOperations.hpp>

#include <memory>
#include <vector>
#include <cstdlib>
#include "init_prof.hpp"

#if SYSTEM_VIEW_ENABLED
#include <SEGGER/SEGGER_SYSVIEW.h>
#endif

void atexit_cleanup_handler()
{
    LOG_INFO("Starting clean up");
}

class Cleanup // TODO is this required? It doesn't seem to do anything productive
{
  public:
    ~Cleanup()
    {
        const int result_2 = std::atexit(atexit_cleanup_handler);
        if (result_2 != 0) {
            LOG_FATAL("at exit failed");
        }
    }
};

int main()
{
    constexpr auto ApplicationName = "PurePhone";

    const std::vector<std::string> fileIndexerAudioPaths = {{purefs::dir::getUserMediaPath()}};

    prof::init();

#if SYSTEM_VIEW_ENABLED
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_ENTER);
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_EXIT);
    SEGGER_SYSVIEW_DisableEvents(SYSVIEW_EVTMASK_ISR_TO_SCHEDULER);
    SEGGER_SYSVIEW_WaitForConnection();
    SEGGER_SYSVIEW_Start();
#endif

    auto platformFactory = purephone::PlatformFactory();
    auto platform        = platformFactory.makePlatform();

    if (!sys::SystemWatchdog::getInstance().init()) {
        LOG_ERROR("System watchdog failed to initialize");
        // wait for the hardware watchdog (initialized in reset ISR) to reset the system
        while (1)
            ;
    }

    std::vector<std::unique_ptr<sys::BaseServiceCreator>> systemServices;
#ifdef ENABLE_SERVICE_EVTMGR
    systemServices.emplace_back(sys::CreatorFor<EventManager>([]() { return diagnosticDumpLogs(); }));
#endif
#ifdef ENABLE_SERVICE_FILEINDEXER
    systemServices.emplace_back(sys::CreatorFor<service::ServiceFileIndexer>(std::move(fileIndexerAudioPaths)));
#endif
#ifdef ENABLE_SERVICE_DB
    systemServices.emplace_back(sys::CreatorFor<ServiceDB>());
#endif
#if ENABLE_GSM == 0
    // For now disable permanently Service cellular when there is no GSM configured
    LOG_INFO("ServiceCellular (GSM) - Disabled");
#else

#ifdef ENABLE_SERVICE_ANTENNA
    systemServices.emplace_back(sys::CreatorFor<ServiceAntenna>());
#endif
#ifdef ENABLE_SERVICE_CELLULAR
    systemServices.emplace_back(sys::CreatorFor<ServiceCellular>());
#endif
#endif

#ifdef ENABLE_SERVICE_AUDIO
    systemServices.emplace_back(sys::CreatorFor<ServiceAudio>());
#endif
#ifdef ENABLE_SERVICE_BLUETOOTH
    systemServices.emplace_back(sys::CreatorFor<ServiceBluetooth>());
#endif
#ifdef ENABLE_SERVICE_DESKTOP
    /// Due to the problem with USB MTP not supporting hierarchical folders structure, we cannot use
    /// 'purefs::dir::getUserMediaPath()'. Instead, we can only pass a specific app folder which is very limiting.
    /// Hopefully, support for hierarchical folders will be added in the future and such a case won't be relevant
    /// anymore.
    systemServices.emplace_back(sys::CreatorFor<ServiceDesktop>(purefs::dir::getUserMediaPath() / "app/music_player"));
#endif
#ifdef ENABLE_SERVICE_TIME
    systemServices.emplace_back(sys::CreatorFor<stm::ServiceTime>(std::make_shared<alarms::AlarmOperationsFactory>()));
#endif
#ifdef ENABLE_SERVICE_EINK
    systemServices.emplace_back(sys::CreatorFor<service::eink::ServiceEink>(service::eink::ExitAction::None));
#endif
#ifdef ENABLE_SERVICE_GUI
    systemServices.emplace_back(
        sys::CreatorFor<service::gui::ServiceGUI>(gui::Size{BOARD_EINK_DISPLAY_RES_X, BOARD_EINK_DISPLAY_RES_Y}));
#endif
#if ENABLE_SERVICE_TEST
    systemServices.emplace_back(sys::CreatorFor<service::test::ServiceTest>());
#endif

    auto sysmgr = std::make_shared<sys::SystemManager>(std::move(systemServices));
    sysmgr->StartSystem(
        [&platform]() {
            LOG_DEBUG("System init");
            try {
                platform->init();
            }
            catch (const std::runtime_error &e) {
                LOG_FATAL("%s", e.what());
                abort();
            }

            Log::Logger::get().init(Log::Application{ApplicationName, GIT_REV, VERSION, GIT_BRANCH});
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
            applications.push_back(app::CreateLauncher<app::ApplicationCall>(app::name_call));
#endif
#ifdef ENABLE_APP_SETTINGS
            applications.push_back(app::CreateLauncher<app::ApplicationSettings>(app::name_settings));
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
#ifdef ENABLE_APP_TEST
            applications.push_back(app::CreateLauncher<app::ApplicationTest>(app::application_test));
#endif

            // start application manager
            return sysmgr->RunSystemService(std::make_shared<app::manager::ApplicationManager>(
                                                service::name::appmgr, std::move(applications), app::name_desktop),
                                            sysmgr.get());
        },
        [&platform] {
            try {
                LOG_DEBUG("System deinit");
                if (shutdownFlushLogs() != 1) {
                    LOG_ERROR("Cannot dump logs");
                }
                platform->deinit();
            }
            catch (const std::runtime_error &e) {
                LOG_FATAL("%s", e.what());
                abort();
            }
            sys::SystemWatchdog::getInstance().deinit();
            return true;
        });

    LOG_INFO("Launching %s ", ApplicationName);
    LOG_INFO("commit: %s version: %s branch: %s", GIT_REV, VERSION, GIT_BRANCH);
    cpp_freertos::Thread::StartScheduler();
    LOG_INFO("Scheduler is terminated properly");
    Cleanup cleanup;

    return 0;
}
