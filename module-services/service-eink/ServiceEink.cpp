// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceEink.hpp"
#include "messages/EinkModeMessage.hpp"
#include <service-gui/Common.hpp>
#include <service-gui/messages/EinkReady.hpp>
#include <time/ScopedTime.hpp>

#include <log/log.hpp>
#include <messages/EinkMessage.hpp>
#include <messages/ImageMessage.hpp>
#include <Service/Bus.hpp>

#include <cstring>
#include <memory>

namespace service::eink
{
    namespace
    {
        constexpr auto ServceEinkStackDepth = 4096 + 1024;
        constexpr std::chrono::milliseconds powerOffTime{3000};
        constexpr auto PowerOffTimerName = "PwrOffTimer";
    } // namespace

    ServiceEink::ServiceEink(const std::string &name, std::string parent)
        : sys::Service(name, parent, ServceEinkStackDepth), currentState{State::Running},
          powerOffTimer(PowerOffTimerName, this, powerOffTime.count(), sys::Timer::Type::SingleShot)
    {
        connect(typeid(EinkModeMessage),
                [this](sys::Message *message) -> sys::MessagePointer { return handleEinkModeChangedMessage(message); });

        connect(typeid(ImageMessage),
                [this](sys::Message *request) -> sys::MessagePointer { return handleImageMessage(request); });
    }

    sys::MessagePointer ServiceEink::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *response)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes ServiceEink::InitHandler()
    {
        LOG_INFO("Initializing");
        if (const auto status = screen.resetAndInit(); status != EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!");
            return sys::ReturnCodes::Failure;
        }

        screen.powerOn();

        auto msg = std::make_shared<service::gui::EinkReady>(screen.getDisplaySize());
        sys::Bus::SendUnicast(msg, service::name::gui, this);

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceEink::DeinitHandler()
    {
        screen.shutdown();
        return sys::ReturnCodes::Success;
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

        if (const auto status = screen.resetAndInit(); status != EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!");
        }
        screen.powerOn();
        screen.powerOff();
    }

    void ServiceEink::suspend()
    {
        setState(State::Suspended);

        powerOffTimer.stop();
        screen.shutdown();
    }

    sys::MessagePointer ServiceEink::handleEinkModeChangedMessage(sys::Message *message)
    {
        const auto msg         = static_cast<service::eink::EinkModeMessage *>(message);
        const auto displayMode = msg->getMode() == service::eink::EinkModeMessage::Mode::Normal
                                     ? EinkDisplayColorMode_e::EinkDisplayColorModeStandard
                                     : EinkDisplayColorMode_e::EinkDisplayColorModeInverted;
        screen.setDisplayMode(displayMode);
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

        powerOffTimer.stop();
        updateDisplay(message->getData(), message->getSize(), message->isDeepRefresh());
        powerOffTimer.reload();
        return std::make_shared<service::eink::ImageDisplayedNotification>(message->getContextId());
    }

    void ServiceEink::updateDisplay(const std::uint8_t *frameBuffer, std::uint32_t bufferSize, bool isDeepRefresh)
    {
        screen.setScreenBuffer(frameBuffer, bufferSize);
        screen.powerOn();

        if (const auto temperature = EinkGetTemperatureInternal(); isDeepRefresh) {
            screen.changeWaveform(EinkWaveforms_e::EinkWaveformGC16, temperature);
            screen.dither();
        }
        else {
            screen.changeWaveform(EinkWaveforms_e::EinkWaveformDU2, temperature);
        }

        if (const auto status = screen.update(); status != EinkOK) {
            LOG_FATAL("Failed to update frame");
        }
        if (const auto status =
                screen.refresh(isDeepRefresh ? EinkDisplayTimingsDeepCleanMode : EinkDisplayTimingsFastRefreshMode);
            status != EinkOK) {
            LOG_FATAL("Failed to refresh frame");
        }
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
