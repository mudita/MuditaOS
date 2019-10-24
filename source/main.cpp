
#include <memory>
#include <list>

#include "../module-gui/gui/core/ImageManager.hpp"
#include "log/log.hpp"
#include "memory/usermem.h"
#include "ticks.hpp"

//module-applications
#include "application-clock/ApplicationClock.hpp"
#include "application-call/ApplicationCall.hpp"
#include "application-viewer/ApplicationViewer.hpp"
#include "application-desktop/ApplicationDesktop.hpp"
#include "application-settings/ApplicationSettings.hpp"
#include "application-notes/ApplicationNotes.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-messages/ApplicationMessages.hpp"

//module-services
#include "service-appmgr/ApplicationManager.hpp"
#include "service-evtmgr/EventManager.hpp"
#include "service-db/ServiceDB.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "service-audio/ServiceAudio.hpp"
#include "service-audio/api/AudioServiceAPI.hpp"
#include "service-bluetooth/ServiceBluetooth.hpp"

//module-bsp
#include "bsp/bsp.hpp"
#include "bsp/rtc/rtc.hpp"
#include "bsp/keyboard/keyboard.hpp"

//module-vfs
#include "vfs.hpp"

//module-sys
#include "SystemManager/SystemManager.hpp"


class vfs vfs;


class BlinkyService : public sys::Service {


public:
    BlinkyService(const std::string &name)
            : sys::Service(name) {
        timer_id = CreateTimer(100, true);
        ReloadTimer(timer_id);
    }

    ~BlinkyService() {
    }

    // Invoked upon receiving data message
    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl,sys::ResponseMessage* resp=nullptr) override {

#if 1 // M.P: left here on purpose
        //auto ret = AudioServiceAPI::PlaybackStart(this,"/home/mateusz/Music/limowreck.mp3");
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

#endif
        //auto ret = AudioServiceAPI::PlaybackStart(this,"sys/audio/limowreck.flac");

        return std::make_shared<sys::ResponseMessage>();

    }

    // Invoked when timer ticked
    void TickHandler(uint32_t id) override {

        static bool laststate = false;

        if(!laststate){
            LOG_FATAL("Entering low power..");
            LPM_EnterLowPowerIdle();
            GPIO_PinWrite(GPIO1,1,0);
            laststate = true;
        }
        else{
            LOG_FATAL("Entering full speed..");
            LPM_EnterFullSpeed();
            GPIO_PinWrite(GPIO1,1,1);
            laststate = false;
        }

#if 0 // M.P: left here on purpose
        LOG_DEBUG("Blinky service tick!");

        stopTimer(timer_id);
        std::shared_ptr<sys::DataMessage> msg = std::make_shared<sys::DataMessage>(static_cast<uint32_t >(MessageType::AudioSetInputGain));

        sys::Bus::SendUnicast(msg,GetName(),this);
#endif

    }


    // Invoked during initialization
    sys::ReturnCodes InitHandler() override {

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes DeinitHandler() override {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

    uint32_t timer_id = 0;
};

#include "board/rt1051/common/clock_config.h"

#define     PAD_CONFIG_SLEW_RATE_SLOW          (0 << 0)
#define     PAD_CONFIG_SLEW_RATE_FAST          (1 << 0)

#define     PAD_CONFIG_DRIVER_DISABLED         (0 << 3)
#define     PAD_CONFIG_DRIVER_STRENGTH_LVL_1   (1 << 3)
#define     PAD_CONFIG_DRIVER_STRENGTH_LVL_2   (2 << 3)
#define     PAD_CONFIG_DRIVER_STRENGTH_LVL_3   (3 << 3)
#define     PAD_CONFIG_DRIVER_STRENGTH_LVL_4   (4 << 3)
#define     PAD_CONFIG_DRIVER_STRENGTH_LVL_5   (5 << 3)
#define     PAD_CONFIG_DRIVER_STRENGTH_LVL_6   (6 << 3)
#define     PAD_CONFIG_DRIVER_STRENGTH_LVL_7   (7 << 3)

#define     PAD_CONFIG_SPEED_SLOW_50MHz        (0 << 6)
#define     PAD_CONFIG_SPEED_MEDIUM_1_100MHz   (1 << 6)
#define     PAD_CONFIG_SPEED_MEDIUM_2_100MHz   (2 << 6)
#define     PAD_CONFIG_SPEED_MAX_200MHz        (3 << 6)

#define     PAD_CONFIG_OPEN_DRAIN_DISABLED     (0 << 11)
#define     PAD_CONFIG_OPEN_DRAIN_ENABLED      (1 << 11)

#define     PAD_CONFIG_PULL_KEEPER_DISABLED    (0 << 12)
#define     PAD_CONFIG_PULL_KEEPER_ENABLED     (1 << 12)

#define     PAD_CONFIG_SELECT_KEEPER           (0 << 13)
#define     PAD_CONFIG_SELECT_PULL             (1 << 13)

#define     PAD_CONFIG_PULL_DOWN_100kOhm       (0 << 14)
#define     PAD_CONFIG_PULL_UP_47kOhm          (1 << 14)
#define     PAD_CONFIG_PULL_UP_100kOhm         (2 << 14)
#define     PAD_CONFIG_PULL_UP_22kOhm          (3 << 14)

#define     PAD_CONFIG_HYSTERESIS_DISABLED     (0 << 16)
#define     PAD_CONFIG_HYSTERESIS_ENABLED      (1 << 16)

int main() {

    bsp::BoardInit();

    LOG_PRINTF("Launching PurePhone..\n");



    IOMUXC_SetPinMux(
            IOMUXC_GPIO_AD_B0_01_GPIO1_IO01,
            0U);

    IOMUXC_SetPinConfig(
            IOMUXC_GPIO_AD_B0_01_GPIO1_IO01,
            PAD_CONFIG_SLEW_RATE_SLOW |
            PAD_CONFIG_DRIVER_STRENGTH_LVL_1 |
            PAD_CONFIG_PULL_KEEPER_ENABLED |
            PAD_CONFIG_SELECT_PULL |
            PAD_CONFIG_PULL_UP_100kOhm |
            PAD_CONFIG_HYSTERESIS_DISABLED);

/*    while(1){
        uint32_t cnt = 0;
        LPM_EnterFullSpeed();

        while(cnt++ < 1000000){
            asm("nop");
        }
        LPM_EnterLowPowerIdle();
        cnt = 0;
        while(cnt++ < 1000000){
            asm("nop");
        }
    }

*//*    LPM_EnterFullSpeed();
    LPM_EnterLowPowerIdle();

    while(1);*/

#if 1
    auto sysmgr = std::make_shared<sys::SystemManager>(5000);

    sysmgr->StartSystem([sysmgr]()->int{

        vfs.Init();

        bool ret = false;

       // ret |= sys::SystemManager::CreateService(std::make_shared<EventManager>("EventManager"), sysmgr.get());
       // ret |= sys::SystemManager::CreateService(std::make_shared<ServiceDB>(), sysmgr.get());
        ret |= sys::SystemManager::CreateService(std::make_shared<BlinkyService>("Blinky"), sysmgr.get());
/*        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceCellular>(), sysmgr.get());
        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceAudio>(), sysmgr.get());
        ret |= sys::SystemManager::CreateService(std::make_shared<ServiceBluetooth>(),sysmgr.get());

        //vector with launchers to applications
        std::vector<std::unique_ptr<app::ApplicationLauncher> > applications;
		applications.push_back(std::unique_ptr<app::ApplicationViewerLauncher>(new app::ApplicationViewerLauncher()));
		applications.push_back(std::unique_ptr<app::ApplicationDesktopLauncher>(new app::ApplicationDesktopLauncher()));
		applications.push_back(std::unique_ptr<app::ApplicationCallLauncher>(new app::ApplicationCallLauncher()));
		applications.push_back(std::unique_ptr<app::ApplicationSettingsLauncher>(new app::ApplicationSettingsLauncher()));
		applications.push_back(std::unique_ptr<app::ApplicationNotesLauncher>(new app::ApplicationNotesLauncher()));
		applications.push_back(app::CreateLauncher<app::ApplicationPhonebook>("ApplicationPhonebook"));
		applications.push_back(app::CreateLauncher<app::ApplicationMessages>("ApplicationMessages"));

		//start application manager
        ret |= sysmgr->CreateService(std::make_shared<sapm::ApplicationManager>("ApplicationManager", sysmgr.get(), applications),
                                     sysmgr.get());*/

        if (ret) {
            return 0;
        }




        return 0;


    });

    cpp_freertos::Thread::StartScheduler();
#endif
    return 1;
}
