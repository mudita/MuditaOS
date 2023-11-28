// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WorkerEvent.hpp"
#include "internal/StaticData.hpp"
#include "internal/key_sequences/KeySequenceMgr.hpp"
#include "internal/key_sequences/PowerOffSequence.hpp"
#include "internal/key_sequences/BedsideLampSequence.hpp"
#include "internal/key_sequences/FrontlightSequence.hpp"
#include "internal/key_sequences/AlarmActivateSequence.hpp"
#include "internal/key_sequences/AlarmDeactivateSequence.hpp"
#include "internal/key_sequences/ResetSequence.hpp"

#include <appmgr/messages/PowerOffPopupRequestParams.hpp>
#include <appmgr/messages/RebootPopupRequestParams.hpp>
#include <evtmgr/EventManager.hpp>
#include <evtmgr/battery/Thresholds.hpp>
#include <service-appmgr/Controller.hpp>
#include <hal/temperature_source/TemperatureSource.hpp>
#include <system/Constants.hpp>
#include <screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/KbdMessage.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <service-evtmgr/WorkerEventCommon.hpp>
#include <sys/messages/AlarmActivationStatusChangeRequest.hpp>
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
    : EventManagerCommon(std::move(logDumpFunction),
                         {.battery{.critical = constants::criticalThreshold, .shutdown = constants::shutdownThreshold},
                          .voltage{.shutdown            = constants::shutdownVoltageThreshold,
                                   .measurementMaxCount = constants::measurementThreshold}},
                         name),
      backlightHandler(settings, this), userActivityHandler(this)
{
    buildKeySequences();

#if CONFIG_ENABLE_TEMP == 1
    temperatureSource = hal::temperature::AbstractTemperatureSource::Factory::create();
    updateTemperature(*temperatureSource);
    onMinuteTick = [this](const time_t) { updateTemperature(*temperatureSource); };
#endif
}

void EventManager::handleKeyEvent(sys::Message *msg)
{
    EventManagerCommon::handleKeyEvent(msg);

    auto kbdMessage = dynamic_cast<sevm::KbdMessage *>(msg);
    if (kbdMessage == nullptr) {
        return;
    }
    if (kbdMessage->key.keyCode == bsp::KeyCodes::JoystickEnter) {
        // Power key notification
        bus.sendUnicast(std::make_shared<sevm::KbdMessage>(), service::name::system_manager);
    }

    if (kbdMessage->key.state == RawKey::State::Pressed || kbdMessage->key.state == RawKey::State::Moved) {
        userActivityHandler.handleUserInput();
        backlightHandler.handleKeyPressed(static_cast<int>(mapKey(static_cast<gui::KeyCode>(kbdMessage->key.keyCode))));
    }

    keySequenceMgr->process(kbdMessage->key);
}

sys::ReturnCodes EventManager::InitHandler()
{
    auto ret    = EventManagerCommon::InitHandler();
    latchStatus = bsp::bell_switches::isLatchPressed() ? sevm::LatchStatus::PRESSED : sevm::LatchStatus::RELEASED;
    return ret;
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
        const auto receivedParams = m->getParams();

        /* Required to avoid false positive maybe-uninitialized on value_or due to GCC bug */
        const auto params = receivedParams.has_value() ? receivedParams.value() : screen_light_control::Parameters();
        backlightHandler.processRequest(m->getAction(), params, m->getSender());

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

    connect(sevm::ScreenLightControlRequestParameters(), [&]([[maybe_unused]] sys::Message *msgl) {
        const screen_light_control::ManualModeParameters params = {backlightHandler.getScreenBrightnessValue()};
        auto msg = std::make_shared<sevm::ScreenLightControlParametersResponse>(
            backlightHandler.getScreenLightState(), backlightHandler.getScreenAutoModeState(), params);
        return msg;
    });

    connect(sevm::LatchStatusRequest(), [&]([[maybe_unused]] sys::Message *msgl) {
        auto msg = std::make_shared<sevm::LatchStatusResponse>(latchStatus);
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

    auto frontlightSeq      = std::make_unique<FrontlightSequence>(*this);
    frontlightSeq->onAction = [this]() { backlightHandler.handleScreenLight(backlight::Type::Frontlight); };
    collection.emplace_back(std::move(frontlightSeq));

    auto bedsideLampSeq      = std::make_unique<BedsideLampSequence>(*this);
    bedsideLampSeq->onAction = [this]() { backlightHandler.handleScreenLight(backlight::Type::BedsideLamp); };
    collection.emplace_back(std::move(bedsideLampSeq));

    auto powerOffSeq      = std::make_unique<PowerOffSequence>(*this);
    powerOffSeq->onAction = [this]() {
        backlightHandler.handleScreenLight(backlight::Type::Frontlight);
        app::manager::Controller::sendAction(
            this, app::manager::actions::ShowPopup, std::make_unique<PowerOffPopupRequestParams>());
    };
    collection.emplace_back(std::move(powerOffSeq));

    auto resetSeq      = std::make_unique<ResetSequence>(*this);
    resetSeq->onAction = [this]() {
        LOG_INFO("Incoming manual reset caused by user!");
        EventManagerCommon::dumpLogsToFile();
    };
    collection.emplace_back(std::move(resetSeq));

    auto alarmActivateSeq      = std::make_unique<AlarmActivateSequence>();
    alarmActivateSeq->onAction = [this]() {
        latchStatus = sevm::LatchStatus::RELEASED;
        bus.sendUnicast(
            std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::ACTIVATED),
            service::name::system_manager);
    };
    collection.emplace_back(std::move(alarmActivateSeq));

    auto alarmDeactivateSeq      = std::make_unique<AlarmDeactivateSequence>();
    alarmDeactivateSeq->onAction = [this]() {
        latchStatus = sevm::LatchStatus::PRESSED;
        bus.sendUnicast(
            std::make_shared<sys::AlarmActivationStatusChangeRequest>(sys::AlarmActivationStatus::DEACTIVATED),
            service::name::system_manager);
    };
    collection.emplace_back(std::move(alarmDeactivateSeq));

    keySequenceMgr = std::make_unique<KeySequenceMgr>(std::move(collection));
}
