
#include <memory>
#include <list>

#include "../module-gui/gui/core/ImageManager.hpp"
#include "log/log.hpp"
#include "memory/usermem.h"
#include "ticks.hpp"
#include "version.hpp"

// module-applications
#include "application-antenna/ApplicationAntenna.hpp"
#include "application-call/ApplicationCall.hpp"
#include "application-calllog/ApplicationCallLog.hpp"
#include "application-clock/ApplicationClock.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "application-notes/ApplicationNotes.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-settings/ApplicationSettings.hpp"
#include "application-special-input/AppSpecialInput.hpp"
#include "application-viewer/ApplicationViewer.hpp"

// module-services
#include "service-appmgr/ApplicationManager.hpp"
#include "service-audio/ServiceAudio.hpp"
#include "service-audio/api/AudioServiceAPI.hpp"
#include "service-bluetooth/ServiceBluetooth.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "service-db/ServiceDB.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-desktop/ServiceDesktop.hpp"
#include "service-evtmgr/Constants.hpp"
#include "service-evtmgr/EventManager.hpp"
#include "service-lwip/ServiceLwIP.hpp"

// module-bsp
#include "bsp/bsp.hpp"
#include "bsp/rtc/rtc.hpp"
#include "bsp/keyboard/keyboard.hpp"

// module-vfs
#include "vfs.hpp"

// module-sys
#include "SystemManager/SystemManager.hpp"

// libphonenumber
#include <phonenumbers/phonenumberutil.h>

class vfs vfs;

class BlinkyService : public sys::Service
{

  public:
    BlinkyService(const std::string &name) : sys::Service(name)
    {
        timerBlinkyID = CreateTimer(5000, true);
        // ReloadTimer(timer_id);
    }

    ~BlinkyService()
    {}
    //    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
    //    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
    //        return std::move(threadResponse->records);
    //    }
    //    else{
    //        return std::make_unique<std::vector<ThreadRecord>>();
    //    }
    // Invoked upon receiving data message
    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override
    {

#if 0 // M.P: left here on purpose
      // auto ret = AudioServiceAPI::PlaybackStart(this,"/home/mateusz/Music/limowreck.mp3");
/*        auto ret = AudioServiceAPI::PlaybackStart(this,"sys/audio/limowreck.flac");
        AudioServiceAPI::Stop(this);
        AudioServiceAPI::PlaybackStart(this,"sys/audio/limowreck.flac");*/
        //vTaskDelay(3000);
        //AudioServiceAPI::SetOutputVolume(this,0.6);
        //auto ret = AudioServiceAPI::RecordingStart(this,"sys/audio/rec1mono.wav");
        //vTaskDelay(3000);
        //ret = AudioServiceAPI::Stop(this);
        //vTaskDelay(500);
        //ret = AudioServiceAPI::PlaybackStart(this,"sys/audio/limowreck.flac");


       //auto ret = AudioServiceAPI::RoutingStart(this);
        /*
         //AudioServiceAPI::RoutingRecordCtrl(this,true);
         vTaskDelay(1000);
         //AudioServiceAPI::RoutingSpeakerPhone(this,true);
         vTaskDelay(2000);
         AudioServiceAPI::Stop(this);*/

        //auto ret = AudioServiceAPI::PlaybackStart(this,"sys/audio/limowreck.flac");

#endif

        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override
    {

#if 0 // M.P: left here on purpose
        LOG_DEBUG("Blinky service tick!");

        stopTimer(timerClockID);
        std::shared_ptr<sys::DataMessage> msg = std::make_shared<sys::DataMessage>(static_cast<uint32_t >(MessageType::AudioSetInputGain));

        sys::Bus::SendUnicast(msg,GetName(),this);
#endif
    }

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override
    {

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes DeinitHandler() override
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
    {
        return sys::ReturnCodes::Success;
    }

    uint32_t timerBlinkyID = 0;
};

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

#if 1
    auto sysmgr = std::make_shared<sys::SystemManager>(5000);

    sysmgr->StartSystem([sysmgr]() -> int {
        /// force initialization of PhonenumberUtil because of its stack usage
        /// otherwise we would end up with an init race and PhonenumberUtil could
        /// be initiated in a task with stack not big enough to handle it
        i18n::phonenumbers::PhoneNumberUtil::GetInstance();

        vfs.Init();

        bool ret = false;

        ret |=
            sys::SystemManager::CreateService(std::make_shared<EventManager>(service::name::evt_manager), sysmgr.get());
        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceDB>(), sysmgr.get());
        ret |= sys::SystemManager::CreateService(std::make_shared<BlinkyService>("Blinky"), sysmgr.get());
#if defined(TARGET_Linux) && not defined(SERIAL_PORT)
        // For now disable pernamenlty Service cellular when there is no GSM configured
        LOG_INFO("ServiceCellular (GSM) - Disabled");
#else
        LOG_INFO("ServiceCellular (GSM) - Enabling");
        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceCellular>(), sysmgr.get());
#endif
        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceAudio>(), sysmgr.get());
        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceBluetooth>(), sysmgr.get());
        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceLwIP>(), sysmgr.get());

        // Service Desktop disabled on master - pulling read on usb driver
        // ret |= sys::SystemManager::CreateService(std::make_shared<ServiceDesktop>(), sysmgr.get());

        // vector with launchers to applications
        std::vector<std::unique_ptr<app::ApplicationLauncher>> applications;
        applications.push_back(app::CreateLauncher<app::ApplicationViewer>(app::name_viewer));
        applications.push_back(app::CreateLauncher<app::ApplicationDesktop>(app::name_desktop, false));
        applications.push_back(app::CreateLauncher<app::ApplicationCall>(app::name_call, false));
        applications.push_back(app::CreateLauncher<app::ApplicationSettings>(app::name_settings));
        applications.push_back(app::CreateLauncher<app::ApplicationNotes>(app::name_notes));
        applications.push_back(app::CreateLauncher<app::ApplicationCallLog>(app::CallLogAppStr));
        applications.push_back(app::CreateLauncher<app::ApplicationPhonebook>(app::name_phonebook));
        applications.push_back(app::CreateLauncher<app::ApplicationMessages>(app::name_messages));
        applications.push_back(app::CreateLauncher<app::AppSpecialInput>(app::special_input, false));
        applications.push_back(app::CreateLauncher<app::ApplicationAntenna>(app::name_antenna));

        // start application manager
        ret |= sysmgr->CreateService(
            std::make_shared<sapm::ApplicationManager>("ApplicationManager", sysmgr.get(), applications), sysmgr.get());

        if (ret) {
            return 0;
        }

        return 0;
    });

    cpp_freertos::Thread::StartScheduler();
#endif
    return 1;
}
