// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/EventManager.hpp>

#include "WorkerEvent.hpp"

#include <evtmgr/EVMessages.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>
#include <sys/SystemManager.hpp>
#include <sys/messages/PhoneModeRequest.hpp>

namespace
{
    constexpr std::array sliderKeyCodes = {
        bsp::KeyCodes::SSwitchUp, bsp::KeyCodes::SSwitchMid, bsp::KeyCodes::SSwitchDown};

    [[nodiscard]] bool isSliderKeyCode(bsp::KeyCodes code)
    {
        return std::find(std::begin(sliderKeyCodes), std::end(sliderKeyCodes), code) != std::end(sliderKeyCodes);
    }
} // namespace

auto EventManager::createEventWorker() -> std::unique_ptr<WorkerEventCommon>
{
    return std::make_unique<purephone::WorkerEvent>(this);
}

void EventManager::initProductEvents()
{
    backlightHandler.init();

    connect(sevm::KeypadBacklightMessage(bsp::keypad_backlight::Action::turnOff), [&](sys::Message *msgl) {
        auto request      = static_cast<sevm::KeypadBacklightMessage *>(msgl);
        auto response     = std::make_shared<sevm::KeypadBacklightResponseMessage>();
        response->success = backlightHandler.processKeypadRequest(request->action);
        return response;
    });

    connect(typeid(sevm::ScreenLightControlMessage), [&](sys::Message *msgl) {
        auto *m = dynamic_cast<sevm::ScreenLightControlMessage *>(msgl);
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

    connect(typeid(sevm::VibratorMessage), [&](sys::Message *msgl) {
        auto request = static_cast<sevm::VibratorMessage *>(msgl);
        processVibratorRequest(request->action, request->repetitionTime);
        return sys::msgHandled();
    });

    connect(typeid(sevm::VibratorLevelMessage), [&](sys::Message *msgl) {
        auto request = static_cast<sevm::VibratorLevelMessage *>(msgl);
        processVibratorLevel(request->vibrationLevel);
        return sys::msgHandled();
    });

    connect(typeid(sevm::SIMTrayMessage), [&](sys::Message *) {
        bus.sendUnicast(std::make_shared<sevm::SIMTrayMessage>(), ServiceCellular::serviceName);
        return sys::MessageNone{};
    });

    connect(sevm::ToggleTorchOnOffMessage(), [&]([[maybe_unused]] sys::Message *msg) {
        toggleTorchOnOff();
        return sys::MessageNone{};
    });

    connect(sevm::ToggleTorchColorMessage(), [&]([[maybe_unused]] sys::Message *msg) {
        toggleTorchColor();
        // Handle only torch short press. Done here as workaround since only app can recognize press length.
        backlightHandler.handleKeyPressed();
        return sys::MessageNone{};
    });

    connect(sevm::RequestPhoneModeForceUpdate(), [&]([[maybe_unused]] sys::Message *msg) {
        auto pureEventWorker = static_cast<purephone::WorkerEvent *>(EventWorker.get());
        pureEventWorker->requestSliderPositionRead();
        return sys::MessageNone{};
    });

    connect(typeid(sevm::TurnOffTorchRequest), [&](sys::Message *msg) {
        toggleTorchOff();
        return sys::MessageNone{};
    });
}

void EventManager::toggleTorchOnOff()
{
    auto state    = bsp::torch::getState();
    auto newState = (state == bsp::torch::State::off) ? bsp::torch::State::on : bsp::torch::State::off;
    bsp::torch::turn(newState, bsp::torch::ColourTemperature::coldest);
}

void EventManager::toggleTorchOff()
{
    bsp::torch::turn(bsp::torch::State::off, bsp::torch::ColourTemperature::coldest);
}

void EventManager::toggleTorchColor()
{
    auto state = bsp::torch::getState();
    if (state == bsp::torch::State::on) {
        auto color    = bsp::torch::getColorTemp();
        auto newColor = (color == bsp::torch::ColourTemperature::coldest) ? bsp::torch::ColourTemperature::warmest
                                                                          : bsp::torch::ColourTemperature::coldest;
        bsp::torch::turn(bsp::torch::State::on, newColor);
    }
}

void EventManager::ProcessCloseReason([[maybe_unused]] sys::CloseReason closeReason)
{
    backlightHandler.deinit();
}

sys::MessagePointer EventManager::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{

    auto responseMessage =
        std::static_pointer_cast<sys::ResponseMessage>(EventManagerCommon::DataReceivedHandler(msgl, resp));

    if (responseMessage->retCode != sys::ReturnCodes::Unresolved) {
        return responseMessage;
    }

    bool handled = false;
    if (msgl->messageType == MessageType::EVMModemStatus) {
        if (auto msg = dynamic_cast<sevm::StatusStateMessage *>(msgl)) {
            auto message   = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMModemStatus);
            message->state = msg->state;
            bus.sendUnicast(message, ServiceCellular::serviceName);
        }
        handled = true;
    }
    else if (msgl->messageType == MessageType::EVMRingIndicator) {
        auto msg = std::make_shared<cellular::UrcIncomingNotification>();
        bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
        handled = true;
    }

    if (handled) {
        return std::make_shared<sys::ResponseMessage>();
    }
    else {
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }
}

void EventManager::handleKeyEvent(sys::Message *msg)
{
    EventManagerCommon::handleKeyEvent(msg);

    const auto kbdMessage = dynamic_cast<sevm::KbdMessage *>(msg);
    if (kbdMessage->key.state == RawKey::State::Pressed) {
        if (kbdMessage->key.keyCode == bsp::KeyCodes::FnRight) {
            // Power key notification
            bus.sendUnicast(std::make_shared<sevm::KbdMessage>(), service::name::system_manager);
        }
    }

    if (kbdMessage->key.state == RawKey::State::Pressed) {
        userActivityHandler.handleUserInput();
        backlightHandler.handleKeyPressed(kbdMessage->key.keyCode);
    }
    else if (kbdMessage->key.state == RawKey::State::Moved) {
        handleKeyMoveEvent(kbdMessage->key);
        backlightHandler.handleKeyPressed();
    }
}

void EventManager::handleKeyMoveEvent(RawKey key)
{
    if (isSliderKeyCode(key.keyCode)) {
        LOG_INFO("Slider position: %s", magic_enum::enum_name(key.keyCode).data());
        const auto mode = sys::SystemManager::translateSliderState(key);
        bus.sendUnicast(std::make_shared<sys::PhoneModeRequest>(mode), service::name::system_manager);
    }
}

void EventManager::processVibratorRequest(bsp::vibrator::Action act, std::chrono::milliseconds RepetitionTime)
{
    switch (act) {
    case bsp::vibrator::Action::pulse:
        vibrator->Pulse();
        break;
    case bsp::vibrator::Action::pulseRepeat:
        vibrator->PulseRepeat(RepetitionTime);
        break;
    case bsp::vibrator::Action::pulseRepeatInfinite:
        vibrator->PulseRepeat();
        break;
    case bsp::vibrator::Action::stop:
        vibrator->PulseRepeatStop();
        break;
    }
}

void EventManager::processVibratorLevel(unsigned int vibrationLevel)
{
    vibrator->SetVibrationLevel(vibrationLevel);
}
