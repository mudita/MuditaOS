// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerEvent.hpp"
#include "internal/StaticData.hpp"
#include "internal/key_sequences/KeySequenceMgr.hpp"
#include "internal/key_sequences/PowerOffSequence.hpp"
#include "internal/key_sequences/AlarmActivateSequence.hpp"
#include "internal/key_sequences/AlarmDeactivateSequence.hpp"
#include "internal/key_sequences/ResetSequence.hpp"

#include <appmgr/messages/PowerOffPopupRequestParams.hpp>
#include <appmgr/messages/RebootPopupRequestParams.hpp>
#include <evtmgr/EventManager.hpp>
#include <service-appmgr/Controller.hpp>
#include <hal/temperature_source/TemperatureSource.hpp>
#include <system/Constants.hpp>
#include <screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/KbdMessage.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/WorkerEventCommon.hpp>
#include <sys/messages/AlarmActivationStatusChangeRequest.hpp>
#include <switches/LatchStatusRequest.hpp>
#include <switches/LatchState.hpp>

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

EventManager::EventManager(LogDumpFunction logDumpFunction, const std::string &name)
    : EventManagerCommon(logDumpFunction, name),
      temperatureSource{hal::temperature::AbstractTemperatureSource::Factory::create()},
      backlightHandler(settings, this), userActivityHandler(std::make_shared<sys::CpuSentinel>(name, this), this)
{
    buildKeySequences();
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

    if (kbdMessage->key.state == RawKey::State::Pressed || kbdMessage->key.state == RawKey::State::Moved) {
        userActivityHandler.handleUserInput();
        backlightHandler.handleKeyPressed(static_cast<int>(mapKey(static_cast<gui::KeyCode>(kbdMessage->key.keyCode))));
    }

    keySequenceMgr->process(kbdMessage->key);
}

auto EventManager::createEventWorker() -> std::unique_ptr<WorkerEventCommon>
{
    return std::make_unique<bell::WorkerEvent>(this);
}

void EventManager::initProductEvents()
{
    backlightHandler.init();

    connect(typeid(sevm::ScreenLightControlMessage), [&](sys::Message *msgl) {
        auto *m           = static_cast<sevm::ScreenLightControlMessage *>(msgl);
        const auto params = m->getParams();
        backlightHandler.processScreenRequest(m->getAction(), params.value_or(screen_light_control::Parameters()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetAutoModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetAutoModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetAutoProgressiveModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetAutoProgressiveModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetManualModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetManualModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(typeid(sevm::ScreenLightSetConstLinearModeParams), [&](sys::Message *msgl) {
        auto *m = static_cast<sevm::ScreenLightSetConstLinearModeParams *>(msgl);
        backlightHandler.processScreenRequest(m->getAction(), screen_light_control::Parameters(m->getParams()));
        return sys::msgHandled();
    });

    connect(sevm::ScreenLightControlRequestParameters(), [&](sys::Message *msgl) {
        screen_light_control::ManualModeParameters params = {backlightHandler.getScreenBrightnessValue()};
        auto msg = std::make_shared<sevm::ScreenLightControlParametersResponse>(
            backlightHandler.getScreenLightState(), backlightHandler.getScreenAutoModeState(), params);
        return msg;
    });

    connect(sevm::LatchStatusRequest(), [&](sys::Message *msgl) {
        sevm::LatchStatus state =
            bsp::bell_switches::isLatchPressed() ? sevm::LatchStatus::PRESSED : sevm::LatchStatus::RELEASED;
        auto msg = std::make_shared<sevm::LatchStatusResponse>(state);
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
void EventManager::buildKeySequences()
{
    KeySequenceMgr::SequenceCollection collection;

    auto powerOffSeq      = std::make_unique<PowerOffSequence>(*this);
    powerOffSeq->onAction = [this]() {
        app::manager::Controller::sendAction(
            this, app::manager::actions::ShowPopup, std::make_unique<PowerOffPopupRequestParams>());
    };
    collection.emplace_back(std::move(powerOffSeq));

    auto resetSeq      = std::make_unique<ResetSequence>(*this);
    resetSeq->onAction = [this]() {
        app::manager::Controller::sendAction(
            this, app::manager::actions::ShowPopup, std::make_unique<RebootPopupRequestParams>());
    };
    collection.emplace_back(std::move(resetSeq));

    auto alarmActivateSeq      = std::make_unique<AlarmActivateSequence>();
    alarmActivateSeq->onAction = [this]() {
        bus.sendUnicast(
            std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::ACTIVATED),
            service::name::system_manager);
    };
    collection.emplace_back(std::move(alarmActivateSeq));

    auto alarmDeactivateSeq      = std::make_unique<AlarmDeactivateSequence>();
    alarmDeactivateSeq->onAction = [this]() {
        bus.sendUnicast(
            std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::DEACTIVATED),
            service::name::system_manager);
    };
    collection.emplace_back(std::move(alarmDeactivateSeq));

    keySequenceMgr = std::make_unique<KeySequenceMgr>(std::move(collection));
}
