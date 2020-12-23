// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceEink.hpp"
#include "messages/EinkModeMessage.hpp"
#include "messages/EinkDMATransfer.hpp"
#include "messages/StateRequest.hpp"
#include "messages/TemperatureUpdate.hpp"
#include <service-gui/Common.hpp>
#include <service-gui/messages/GUIDisplayReady.hpp>
#include <time/ScopedTime.hpp>

#include <log/log.hpp>
#include <messages/EinkMessage.hpp>
#include <messages/ImageMessage.hpp>
#include <Service/Bus.hpp>

#include <cstring>
#include <memory>

namespace eink
{
    namespace
    {
        constexpr auto ServceEinkStackDepth = 4096 + 1024;
    } // namespace

    ServiceEink::ServiceEink(const std::string &name, std::string parent)
        : sys::Service(name, parent, ServceEinkStackDepth), selfRefereshTriggerCount{0},
          temperatureMeasurementTriggerCount{0}, powerOffTriggerCount{0},
          powerOffTimer("PwrOffTimer", this, 3000, sys::Timer::Type::SingleShot)
    {
        connect(typeid(service::eink::EinkModeMessage),
                [this](sys::Message *message) -> sys::MessagePointer { return handleEinkModeChangedMessage(message); });

        connect(typeid(service::eink::EinkDMATransfer),
                [this](sys::Message *request) -> sys::MessagePointer { return handleEinkDMATransfer(request); });

        connect(typeid(service::eink::ImageMessage),
                [this](sys::Message *request) -> sys::MessagePointer { return handleImageMessage(request); });

        connect(typeid(service::eink::StateRequest),
                [this](sys::Message *request) -> sys::MessagePointer { return handleStateRequest(request); });

        connect(typeid(service::eink::TemperatureUpdate),
                [this](sys::Message *request) -> sys::MessagePointer { return handleTemperatureUpdate(request); });
    }

    sys::MessagePointer ServiceEink::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::ReturnCodes ServiceEink::InitHandler()
    {
        LOG_INFO("[ServiceEink] Initializing");
        if (const auto status = screen.resetAndInit(); status != EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!\n");
            return sys::ReturnCodes::Failure;
        }
        screen.powerOn();

        auto msg = std::make_shared<service::gui::GUIDisplayReady>(suspendInProgress, shutdownInProgress);
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
        LOG_FATAL("[ServiceEink] PowerModeHandler: %s", c_str(mode));

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
        suspended = false;

        if (const auto status = screen.resetAndInit(); status != EinkOK) {
            LOG_FATAL("Error: Could not initialize Eink display!\n");
        }
        screen.powerOn();
        screen.powerOff();
    }

    void ServiceEink::suspend()
    {
        suspended = true;

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

    sys::MessagePointer ServiceEink::handleEinkDMATransfer(sys::Message *message)
    {
        utils::time::Scoped measurement("EinkDMATransfer");

        if (suspended) {
            if (suspendInProgress) {
                LOG_ERROR("drawing before suspend failed");
                suspendInProgress = false;
            }
            LOG_INFO("[ServiceEink] Received image while suspended, ignoring");
            return sys::MessageNone{};
        }

        screen.powerOn();
        if (const auto temperature = EinkGetTemperatureInternal(); deepRefresh) {
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
                screen.refresh(deepRefresh ? EinkDisplayTimingsDeepCleanMode : EinkDisplayTimingsFastRefreshMode);
            status != EinkOK) {
            LOG_FATAL("Failed to refresh frame");
        }

        powerOffTimer.reload();

        auto msg           = std::make_shared<service::gui::GUIDisplayReady>(suspendInProgress, shutdownInProgress);
        suspendInProgress  = false;
        shutdownInProgress = false;
        sys::Bus::SendUnicast(msg, service::name::gui, this);

        return sys::MessageNone{};
    }

    sys::MessagePointer ServiceEink::handleImageMessage(sys::Message *request)
    {
        auto message = static_cast<service::eink::ImageMessage *>(request);

        powerOffTimer.stop();
        screen.setScreenBuffer(message->getData(), message->getSize());
        deepRefresh = message->getDeepRefresh();

        shutdownInProgress = message->getShutdown();
        if (shutdownInProgress) {
            LOG_DEBUG("Shutdown In Progress");
        }
        suspendInProgress = message->getSuspend();
        if (suspendInProgress) {
            LOG_DEBUG("Suspend In Progress");
        }

        sys::Bus::SendUnicast(std::make_shared<service::eink::EinkDMATransfer>(), GetName(), this);
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::MessagePointer ServiceEink::handleStateRequest(sys::Message *)
    {
        return std::make_shared<service::gui::GUIDisplayReady>(suspendInProgress, shutdownInProgress);
    }

    sys::MessagePointer ServiceEink::handleTemperatureUpdate(sys::Message *)
    {
        return nullptr;
    }
} // namespace eink
