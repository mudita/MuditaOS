// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationWhatsNew.hpp"
#include "WhatsNewCommon.hpp"

#include "windows/WhatsNewWindow.hpp"
#include "presenter/WhatsNewPresenter.hpp"
#include "models/WhatsNewModel.hpp"

#include <common/windows/AppsBatteryStatusWindow.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace app
{
    ApplicationWhatsNew::ApplicationWhatsNew(std::string name,
                                             std::string parent,
                                             StatusIndicators statusIndicators,
                                             StartInBackground startInBackground,
                                             std::uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth)
    {}

    ApplicationWhatsNew::~ApplicationWhatsNew()
    {
        cpuSentinel->BlockWfiMode(false);
    }

    sys::ReturnCodes ApplicationWhatsNew::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        whatsNewModel = std::make_unique<whatsNew::models::WhatsNewModel>();

        batteryModel                 = std::make_unique<app::BatteryModel>(this);
        lowBatteryInfoModel          = std::make_unique<app::LowBatteryInfoModel>();
        cpuSentinel                  = std::make_shared<sys::CpuSentinel>(applicationWhatsNewName, this);
        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);
        cpuSentinel->BlockWfiMode(true);

        createUserInterface();

        return sys::ReturnCodes::Success;
    }

    void ApplicationWhatsNew::createUserInterface()
    {
        windowsFactory.attach(whatsNew::window::name::main, [this](ApplicationCommon *app, const std::string &name) {
            auto presenter = std::make_unique<app::whatsNew::WhatsNewPresenter>(*batteryModel, *lowBatteryInfoModel);
            return std::make_unique<whatsNew::WhatsNewWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(whatsNew::window::name::whatsNewLowBattery,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::AppsBatteryStatusWindow>(app, name);
                              });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification});
    }

    sys::MessagePointer ApplicationWhatsNew::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            response != nullptr && response->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        return handleAsyncResponse(resp);
    }
} // namespace app