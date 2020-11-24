// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

// services
#include <service-appmgr/ApplicationManager.hpp>
#include <service-audio/ServiceAudio.hpp>
#include <service-bluetooth/ServiceBluetooth.hpp>
#include <service-db/ServiceDB.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EventManager.hpp>
#include <service-lwip/ServiceLwIP.hpp>
#include <service-time/ServiceTime.hpp>
#include <service-fileindexer/Constants.hpp>
#include <service-fileindexer/ServiceFileIndexer.hpp>

#if ENABLE_GSM == 1
#include <service-fota/ServiceFota.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-antenna/ServiceAntenna.hpp>
#endif

#include <bsp/bsp.hpp>
#include <Application.hpp>
#include <ApplicationLauncher.hpp>
#include <log/log.hpp>
#include <phonenumbers/phonenumberutil.h>
#include <source/version.hpp>
#include <SystemManager/SystemManager.hpp>
#include <thread.hpp>
#include <vfs.hpp>
#include <utf8/UTF8.hpp>

#include <memory>
#include <vector>

class vfs vfs;

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

    LOG_PRINTF("Launching PurePhone \n");
    LOG_PRINTF("commit: %s tag: %s branch: %s\n", GIT_REV, GIT_TAG, GIT_BRANCH);

    auto sysmgr = std::make_shared<sys::SystemManager>(5000);

    sysmgr->StartSystem([sysmgr]() {
        /// force initialization of PhonenumberUtil because of its stack usage
        /// otherwise we would end up with an init race and PhonenumberUtil could
        /// be initiated in a task with stack not big enough to handle it
        i18n::phonenumbers::PhoneNumberUtil::GetInstance();

        vfs.Init();

        auto ret = true;

        ret &=
            sys::SystemManager::CreateService(std::make_shared<EventManager>(service::name::evt_manager), sysmgr.get());
        ret &= sys::SystemManager::CreateService(
            std::make_shared<service::ServiceFileIndexer>(service::name::file_indexer), sysmgr.get());
        ret &= sys::SystemManager::CreateService(std::make_shared<ServiceDB>(), sysmgr.get());

#if ENABLE_GSM == 0
        // For now disable pernamenlty Service cellular when there is no GSM configured
        LOG_INFO("ServiceCellular (GSM) - Disabled");
#else
        LOG_INFO("ServiceCellular (GSM) - Enabling");
        ret &= sys::SystemManager::CreateService(std::make_shared<ServiceAntenna>(), sysmgr.get());
        ret &= sys::SystemManager::CreateService(std::make_shared<ServiceCellular>(), sysmgr.get());
        ret &= sys::SystemManager::CreateService(std::make_shared<FotaService::Service>(), sysmgr.get());
#endif

        ret &= sys::SystemManager::CreateService(std::make_shared<ServiceAudio>(), sysmgr.get());
        ret &= sys::SystemManager::CreateService(std::make_shared<ServiceBluetooth>(), sysmgr.get());
        ret &= sys::SystemManager::CreateService(std::make_shared<ServiceLwIP>(), sysmgr.get());

        ret &= sys::SystemManager::CreateService(std::make_shared<stm::ServiceTime>(), sysmgr.get());
        ret &= sys::SystemManager::CreateService(std::make_shared<ServiceDesktop>(), sysmgr.get());

        // vector with launchers to applications
        std::vector<std::unique_ptr<app::ApplicationLauncher>> applications;
#ifdef ENABLE_APP_DESKTOP
        applications.push_back(app::CreateLauncher<app::ApplicationDesktop>(app::name_desktop, false));
#endif
#ifdef ENABLE_APP_CALL
        applications.push_back(app::CreateLauncher<app::ApplicationCall>(app::name_call, false));
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
        applications.push_back(app::CreateLauncher<app::ApplicationSpecialInput>(app::special_input, false));
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
        applications.push_back(app::CreateLauncher<app::ApplicationMeditation>(app::name_meditation));
#endif
#ifdef ENABLE_APP_CALCULATOR
        applications.push_back(app::CreateLauncher<app::ApplicationCalculator>(app::name_calculator));
#endif

        // start application manager
        ret &= sysmgr->CreateService(
            std::make_shared<app::manager::ApplicationManager>(
                app::manager::ApplicationManager::ServiceName, std::move(applications), app::name_desktop),
            sysmgr.get());

        return ret;
    });

    cpp_freertos::Thread::StartScheduler();

    return 1;
}
