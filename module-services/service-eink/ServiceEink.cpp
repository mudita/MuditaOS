// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <board.h>
#include "ServiceEink.hpp"
#include "messages/EinkModeMessage.hpp"
#include "messages/PrepareDisplayEarlyRequest.hpp"
#include <service-gui/messages/EinkInitialized.hpp>
#include <time/ScopedTime.hpp>
#include <Timers/TimerFactory.hpp>

#include <log.hpp>
#include <messages/EinkMessage.hpp>
#include <messages/ImageMessage.hpp>
#include <SystemManager/messages/DeviceRegistrationMessage.hpp>
#include <SystemManager/messages/SentinelRegistrationMessage.hpp>
#include <SystemManager/Constants.hpp>

#include <cstring>
#include <memory>
#include <gsl/util>

namespace service::eink
{
    namespace
    {
        constexpr auto ServceEinkStackDepth = 2048U;
        constexpr std::chrono::milliseconds displayPowerOffTimeout{3800};
    } // namespace

    ServiceEink::ServiceEink(const std::string &name, std::string parent)
        : sys::Service(name, std::move(parent), ServceEinkStackDepth),
          display{{BOARD_EINK_DISPLAY_RES_X, BOARD_EINK_DISPLAY_RES_Y}}, currentState{State::Running}
    {
        displayPowerOffTimer = sys::TimerFactory::createSingleShotTimer(
            this, "einkDisplayPowerOff", displayPowerOffTimeout, [this](sys::Timer &) { display.powerOff(); });
        connect(typeid(EinkModeMessage),
                [this](sys::Message *message) -> sys::MessagePointer { return handleEinkModeChangedMessage(message); });

        connect(typeid(ImageMessage),
                [this](sys::Message *request) -> sys::MessagePointer { return handleImageMessage(request); });

        connect(typeid(PrepareDisplayEarlyRequest),
                [this](sys::Message *request) -> sys::MessagePointer { return handlePrepareEarlyRequest(request); });

        eInkSentinel = std::make_shared<EinkSentinel>(name::eink, this);
        display.setEinkSentinel(eInkSentinel);
    }

    sys::MessagePointer ServiceEink::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *response)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes ServiceEink::InitHandler()
    {
        LOG_INFO("Initializing");
        if (const auto status = display.resetAndInit(); status != EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!");
            return sys::ReturnCodes::Failure;
        }

        auto deviceRegistrationMsg = std::make_shared<sys::DeviceRegistrationMessage>(display.getDevice());
        bus.sendUnicast(deviceRegistrationMsg, service::name::system_manager);

        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(eInkSentinel);
        bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

        display.powerOn();

        auto msg = std::make_shared<service::gui::EinkInitialized>(display.getSize());
        bus.sendUnicast(msg, service::name::gui);
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceEink::DeinitHandler()
    {
        display.wipeOut();
        display.shutdown();
        return sys::ReturnCodes::Success;
    }

    void ServiceEink::ProcessCloseReason(sys::CloseReason closeReason)
    {
        sendCloseReadyMessage(this);
    }

    sys::ReturnCodes ServiceEink::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_INFO("PowerModeHandler: %s", c_str(mode));
        switch (mode) {
        case sys::ServicePowerMode::Active:
            enterActiveMode();
            break;
        case sys::ServicePowerMode::SuspendToRAM:
            [[fallthrough]];
        case sys::ServicePowerMode::SuspendToNVM:
            suspend();
            break;
        }
        return sys::ReturnCodes::Success;
    }

    void ServiceEink::enterActiveMode()
    {
        setState(State::Running);

        if (const auto status = display.resetAndInit(); status != EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!");
        }
        display.powerOn();
        display.powerOff();
    }

    void ServiceEink::suspend()
    {
        setState(State::Suspended);
        display.shutdown();
    }

    sys::MessagePointer ServiceEink::handleEinkModeChangedMessage(sys::Message *message)
    {
        const auto msg         = static_cast<service::eink::EinkModeMessage *>(message);
        const auto displayMode = msg->getMode() == service::eink::EinkModeMessage::Mode::Normal
                                     ? EinkDisplayColorMode_e::EinkDisplayColorModeStandard
                                     : EinkDisplayColorMode_e::EinkDisplayColorModeInverted;
        display.setMode(displayMode);
        return sys::MessageNone{};
    }

    sys::MessagePointer ServiceEink::handleImageMessage(sys::Message *request)
    {
        const auto message = static_cast<service::eink::ImageMessage *>(request);
        if (isInState(State::Suspended)) {
            LOG_WARN("Received image while suspended, ignoring");
            return sys::MessageNone{};
        }
        utils::time::Scoped measurement("ImageMessage");

        showImage(message->getData(), message->getRefreshMode());
        return std::make_shared<service::eink::ImageDisplayedNotification>(message->getContextId());
    }

    void ServiceEink::showImage(std::uint8_t *frameBuffer, ::gui::RefreshModes refreshMode)
    {
        displayPowerOffTimer.stop();

        auto displayPowerOffTimerReload = gsl::finally([this]() { displayPowerOffTimer.start(); });

        if (const auto status = prepareDisplay(refreshMode, WaveformTemperature::KEEP_CURRENT);
            status != EinkStatus_e ::EinkOK) {
            LOG_FATAL("Failed to prepare frame");
            return;
        }

        if (const auto status = updateDisplay(frameBuffer, refreshMode); status != EinkStatus_e ::EinkOK) {
            LOG_FATAL("Failed to update frame");
            return;
        }

        if (const auto status = refreshDisplay(refreshMode); status != EinkStatus_e ::EinkOK) {
            LOG_FATAL("Failed to refresh frame");
            return;
        }
    }

    EinkStatus_e ServiceEink::updateDisplay(std::uint8_t *frameBuffer, ::gui::RefreshModes refreshMode)
    {
        return display.update(frameBuffer);
    }

    EinkStatus_e ServiceEink::refreshDisplay(::gui::RefreshModes refreshMode)
    {
        const auto isDeepRefresh = refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP;
        return display.refresh(isDeepRefresh ? EinkDisplayTimingsDeepCleanMode : EinkDisplayTimingsFastRefreshMode);
    }

    EinkStatus_e ServiceEink::prepareDisplay(::gui::RefreshModes refreshMode, WaveformTemperature behaviour)
    {
        EinkStatus_e status;

        displayPowerOffTimer.stop();
        display.powerOn();

        const auto temperature = behaviour == WaveformTemperature::KEEP_CURRENT ? display.getLastTemperature()
                                                                                : EinkGetTemperatureInternal();

        if (refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP) {
            status = display.setWaveform(EinkWaveforms_e::EinkWaveformGC16, temperature);
            if (status == EinkOK) {
                display.dither();
            }
        }
        else {
            status = display.setWaveform(EinkWaveforms_e::EinkWaveformDU2, temperature);
        }
        return status;
    }

    sys::MessagePointer ServiceEink::handlePrepareEarlyRequest(sys::Message *message)
    {
        const auto waveformUpdateMsg = static_cast<service::eink::PrepareDisplayEarlyRequest *>(message);
        prepareDisplay(waveformUpdateMsg->getRefreshMode(), WaveformTemperature::MEASURE_NEW);
        return sys::MessageNone{};
    }

    void ServiceEink::setState(State state) noexcept
    {
        currentState = state;
    }

    bool ServiceEink::isInState(State state) const noexcept
    {
        return currentState == state;
    }

} // namespace service::eink
