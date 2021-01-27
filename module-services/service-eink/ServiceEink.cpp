// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceEink.hpp"
#include "messages/EinkModeMessage.hpp"
#include "messages/PrepareDisplayEarlyRequest.hpp"
#include <service-gui/Common.hpp>
#include <service-gui/messages/EinkInitialized.hpp>
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
    } // namespace

    ServiceEink::ServiceEink(const std::string &name, std::string parent)
        : sys::Service(name, parent, ServceEinkStackDepth), currentState{State::Running}
    {
        connect(typeid(EinkModeMessage),
                [this](sys::Message *message) -> sys::MessagePointer { return handleEinkModeChangedMessage(message); });

        connect(typeid(ImageMessage),
                [this](sys::Message *request) -> sys::MessagePointer { return handleImageMessage(request); });

        connect(typeid(PrepareDisplayEarlyRequest),
                [this](sys::Message *request) -> sys::MessagePointer { return handlePrepareEarlyRequest(request); });
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

        display.powerOn();

        auto msg = std::make_shared<service::gui::EinkInitialized>(display.getSize());
        sys::Bus::SendUnicast(msg, service::name::gui, this);

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ServiceEink::DeinitHandler()
    {
        display.shutdown();
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

        updateDisplay(message->getData(), message->getRefreshMode());
        return std::make_shared<service::eink::ImageDisplayedNotification>(message->getContextId());
    }

    void ServiceEink::updateDisplay(std::uint8_t *frameBuffer, ::gui::RefreshModes refreshMode)
    {
        prepareDisplay(refreshMode);

        if (const auto status = display.update(frameBuffer); status != EinkOK) {
            LOG_FATAL("Failed to update frame");
        }

        const auto isDeepRefresh = refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP;
        if (const auto status =
                display.refresh(isDeepRefresh ? EinkDisplayTimingsDeepCleanMode : EinkDisplayTimingsFastRefreshMode);
            status != EinkOK) {
            LOG_FATAL("Failed to refresh frame");
        }
    }

    void ServiceEink::prepareDisplay(::gui::RefreshModes refreshMode)
    {
        display.powerOn();

        const auto temperature = EinkGetTemperatureInternal();
        if (refreshMode == ::gui::RefreshModes::GUI_REFRESH_DEEP) {
            display.setWaveform(EinkWaveforms_e::EinkWaveformGC16, temperature);
            display.dither();
        }
        else {
            display.setWaveform(EinkWaveforms_e::EinkWaveformDU2, temperature);
        }
    }

    sys::MessagePointer ServiceEink::handlePrepareEarlyRequest(sys::Message *message)
    {
        const auto waveformUpdateMsg = static_cast<service::eink::PrepareDisplayEarlyRequest *>(message);
        prepareDisplay(waveformUpdateMsg->getRefreshMode());
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
