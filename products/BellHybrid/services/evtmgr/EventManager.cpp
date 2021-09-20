// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "internal/StaticData.hpp"

#include "WorkerEvent.hpp"
#include <evtmgr/EventManager.hpp>
#include <keymap/KeyMap.hpp>
#include <module-bsp/hal/temperature_source/TemperatureSource.hpp>
#include <SystemManager/Constants.hpp>
#include <screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/KbdMessage.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/WorkerEventCommon.hpp>
#include <sys/messages/AlarmActivationStatusChangeRequest.hpp>

namespace
{
    auto updateTemperature = [](hal::temperature::AbstractTemperatureSource &source) {
        const auto temp = source.read();
        if (not temp) {
            LOG_FATAL("Error during reading from temperature source");
        }
        else {
            evtmgr::internal::StaticData::get().setCurrentTemperature(*temp);
        };
    };
}

EventManager::EventManager(const std::string &name)
    : EventManagerCommon(name), temperatureSource{hal::temperature::AbstractTemperatureSource::Factory::create()},
      backlightHandler(settings, this)
{
    updateTemperature(*temperatureSource);

    onMinuteTick = [this](const time_t) { updateTemperature(*temperatureSource); };
}

void EventManager::handleKeyEvent(sys::Message *msg)
{
    EventManagerCommon::handleKeyEvent(msg);

    auto kbdMessage = dynamic_cast<sevm::KbdMessage *>(msg);
    if (kbdMessage == nullptr) {
        return;
    }

    auto key = mapKey(static_cast<gui::KeyCode>(kbdMessage->key.keyCode));

    if (kbdMessage->key.state == RawKey::State::Released) {
        if (key == KeyMap::DeepPressUp) {
            bus.sendUnicast(
                std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::ACTIVATED),
                service::name::system_manager);
        }
        else if (key == KeyMap::DeepPressDown) {
            bus.sendUnicast(
                std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::DEACTIVATED),
                service::name::system_manager);
        }
    }
}

auto EventManager::createEventWorker() -> std::unique_ptr<WorkerEventCommon>
{
    return std::make_unique<bell::WorkerEvent>(this);
}

void EventManager::initProductEvents()
{
    backlightHandler.init();

    connect(typeid(sevm::ScreenLightControlMessage), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightControlMessage *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters());
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetAutoModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetAutoModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetManualModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetManualModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(sevm::ScreenLightControlRequestParameters(), [&](sys::Message *msgl) {
        screen_light_control::ManualModeParameters params = {backlightHandler.getScreenBrightnessValue()};
        auto msg = std::make_shared<sevm::ScreenLightControlParametersResponse>(
            backlightHandler.getScreenLightState(), backlightHandler.getScreenAutoModeState(), params);
        return msg;
    });
}

sys::MessagePointer EventManager::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{

    auto responseMessage =
        std::static_pointer_cast<sys::ResponseMessage>(EventManagerCommon::DataReceivedHandler(msgl, resp));

    if (responseMessage->retCode != sys::ReturnCodes::Unresolved) {
        return responseMessage;
    }

    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}
