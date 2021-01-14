﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-antenna/ServiceAntenna.hpp"
#include "service-antenna/AntennaMessage.hpp"
#include "service-antenna/AntennaServiceAPI.hpp"

#include <service-appmgr/model/ApplicationManager.hpp>

#include <at/response.hpp>
#include <common_data/EventStore.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <module-utils/state/ServiceState.hpp>
#include <projdefs.h>
#include <Service/Timer.hpp>
#include <service-cellular/State.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-cellular/CellularMessage.hpp>

#include <ticks.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

const char *ServiceAntenna::serviceName = "ServiceAntenna";

namespace antenna
{
    const char *c_str(antenna::State state)
    {
        switch (state) {
        case antenna::State::none:
            return "none";
        case antenna::State::init:
            return "init";
        case antenna::State::connectionStatus:
            return "connectionStatus";
        case antenna::State::switchAntenna:
            return "switchAntenna";
        case antenna::State::signalCheck:
            return "signalCheck";
        case antenna::State::bandCheck:
            return "bandCheck";
        case antenna::State::idle:
            return "idle";
        case antenna::State::csqChange:
            return "csqChange";
        case antenna::State::locked:
            return "locked";
        }
        return "";
    }
} // namespace antenna

ServiceAntenna::ServiceAntenna() : sys::Service(serviceName)
{
    LOG_INFO("[%s] Initializing", serviceName);

    timer = std::make_unique<sys::Timer>("Antena", this, 5000, sys::Timer::Type::Periodic);
    timer->connect([&](sys::Timer &) {
        timer->stop();
        auto stateToSet = state->get();
        if (state->timeoutOccured(cpp_freertos::Ticks::GetTicks())) {
            LOG_WARN("State [ %s ] timeout occured, setting [ %s ] state",
                     c_str(state->get()),
                     c_str(state->getTimeoutState()));
            stateToSet = state->getTimeoutState();
        }
        state->set(stateToSet);
    });

    state = new utils::state::State<antenna::State>(this);

    bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);
    bus.channels.push_back(sys::BusChannel::AntennaNotifications);
}

ServiceAntenna::~ServiceAntenna()
{
    LOG_INFO("[%s] Cleaning resources", serviceName);
}

// Invoked upon receiving data message
sys::MessagePointer ServiceAntenna::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    bool handled = false;

    switch (msgl->messageType) {
    case MessageType::StateChange:
        HandleStateChange(state->get());
        break;
    case MessageType::AntennaChanged: {
        bsp::cellular::antenna antenna;
        if (CellularServiceAPI::GetAntenna(this, antenna)) {
            currentAntenna = antenna;
            if (state->get() == antenna::State::switchAntenna) {
                LOG_INFO("Antena switched.");

                state->enableStateTimeout(cpp_freertos::Ticks::GetTicks(),
                                          pdMS_TO_TICKS(antenna::connectionStatusTimeout),
                                          antenna::State::switchAntenna);
                state->set(antenna::State::connectionStatus);
            }
        }
        handled = true;
        break;
    }
    case MessageType::CellularNotification: {

        auto msg = dynamic_cast<CellularNotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->type == CellularNotificationMessage::Type::CallAborted) {
                AntennaServiceAPI::LockRequest(this, antenna::lockState::unlocked);
            }
        }
        handled = true;
        break;
    }
    case MessageType::CellularCall: {
        auto msg = dynamic_cast<CellularCallMessage *>(msgl);
        if (msg != nullptr) {
            AntennaServiceAPI::LockRequest(this, antenna::lockState::locked);
        }
    } break;
    case MessageType::CellularStateRequest: {
        auto msg = dynamic_cast<cellular::StateChange *>(msgl);
        if (msg != nullptr) {
            if (msg->request == cellular::State::ST::Ready) {
                state->set(antenna::State::init);
            }
        }
    } break;
    case MessageType::AntennaCSQChange:
        LOG_WARN("CSQChange message");
        if (state->get() == antenna::State::idle) {
            state->set(antenna::State::csqChange);
        }
        break;
    case MessageType::AntennaLockService: {
        auto msg = dynamic_cast<AntennaLockRequestMessage *>(msgl);
        if (msg != nullptr) {
            handleLockRequest(msg->request);
            handled = true;
        }
    } break;
    case MessageType::AntennaGetLockState: {
        auto responseMessage = std::make_shared<AntennaLockRequestResponse>(true, serviceLocked);
        return responseMessage;
    } break;
    default:
        break;
    }

    if (handled)
        return std::make_shared<sys::ResponseMessage>();
    else
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

// Invoked during initialization
sys::ReturnCodes ServiceAntenna::InitHandler()
{
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceAntenna::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceAntenna::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceEvtMgr] PowerModeHandler: %s", c_str(mode));

    suspended = true;

    switch (mode) {
    case sys::ServicePowerMode ::Active:
        suspended = false;
        break;
    case sys::ServicePowerMode ::SuspendToRAM:
    case sys::ServicePowerMode ::SuspendToNVM:
        break;
    }

    return sys::ReturnCodes::Success;
}

void ServiceAntenna::handleLockRequest(antenna::lockState request)
{
    auto currentState = state->get();
    if (request == antenna::lockState::locked) {
        LOG_INFO("AntennaService lock request.");
        if (currentState != antenna::State::locked) {
            state->set(antenna::State::locked);
            serviceLocked = request;
            LOG_INFO("AntennaService locked.");
            return;
        }
        LOG_INFO("AntennaService lock request skipped.");
    }
    else if (request == antenna::lockState::unlocked) {
        LOG_INFO("AntennaService unlock request. Service state: %s", c_str(state->get()));
        if (currentState == antenna::State::locked) {
            state->set(state->getLast());
            serviceLocked = request;
            LOG_INFO("AntennaService unlocked. Restoring last state.");
            return;
        }
        LOG_INFO("AntennaService unlock request skipped.");
    }
}

bool ServiceAntenna::HandleStateChange(antenna::State state)
{
    bool ret = false;
    switch (state) {
    case antenna::State::none:
        ret = noneStateHandler();
        ;
        break;
    case antenna::State::init:
        ret = initStateHandler();
        break;
    case antenna::State::connectionStatus:
        ret = connectionStatusStateHandler();
        break;
    case antenna::State::switchAntenna:
        ret = switchAntennaStateHandler();
        break;
    case antenna::State::signalCheck:
        ret = signalCheckStateHandler();
        break;
    case antenna::State::bandCheck:
        ret = bandCheckStateHandler();
        break;
    case antenna::State::idle:
        ret = idleStateHandler();
        break;
    case antenna::State::csqChange:
        ret = csqChangeStateHandler();
        break;
    case antenna::State::locked:
        ret = lockedStateHandler();
        break;
    }
    if (!ret) {
        LOG_WARN("State [ %s ] not handled. Reloading timer.", c_str(state));
        timer->reload();
        timer->start();
    }
    return ret;
}

bool ServiceAntenna::initStateHandler(void)
{
    LOG_INFO("State Init");
    bsp::cellular::antenna antenna;
    if (CellularServiceAPI::GetAntenna(this, antenna)) {
        currentAntenna = antenna;
        state->enableStateTimeout(cpp_freertos::Ticks::GetTicks(),
                                  pdMS_TO_TICKS(antenna::connectionStatusTimeout),
                                  antenna::State::switchAntenna);
        state->set(antenna::State::connectionStatus);
        return true;
    }
    return false;
}

bool ServiceAntenna::noneStateHandler(void)
{
    return true;
}

bool ServiceAntenna::connectionStatusStateHandler(void)
{
    auto status = Store::GSM::get()->getNetwork().status;
    if (status == Store::Network::Status::RegisteredHomeNetwork ||
        status == Store::Network::Status::RegisteredRoaming) {
        LOG_INFO("Modem connected.");
        state->disableTimeout();
        state->set(antenna::State::bandCheck);
        return true;
    }
    LOG_INFO("Modem not connected.");
    return false;
}

bool ServiceAntenna::switchAntennaStateHandler(void)
{
    if (currentAntenna == bsp::cellular::antenna::highBand) {
        CellularServiceAPI::SelectAntenna(this, bsp::cellular::antenna::lowBand);
        return true;
    }
    else if (currentAntenna == bsp::cellular::antenna::lowBand) {
        CellularServiceAPI::SelectAntenna(this, bsp::cellular::antenna::highBand);
        return true;
    }
    return false;
}

bool ServiceAntenna::signalCheckStateHandler(void)
{
    std::string csq;
    uint32_t csqValue = 0;
    if (CellularServiceAPI::GetCSQ(this, csq)) {
        at::response::parseCSQ(csq, csqValue);
        if (csqValue <= antenna::signalTreshold) {
            LOG_INFO("Signal strength below treshold. Switch antenna");
            state->set(antenna::State::switchAntenna);
            return true;
        }
        lastCsq = csqValue;
        state->set(antenna::State::idle);
        return true;
    }

    return false;
}
bool ServiceAntenna::bandCheckStateHandler(void)
{
    std::string cellularResponse;
    if (CellularServiceAPI::GetQNWINFO(this, cellularResponse)) {
        std::string band;
        at::response::parseQNWINFO(cellularResponse, band);

        auto bandFrequency     = at::response::qnwinfo::parseNetworkFrequency(band);
        constexpr uint32_t GHz = 1000;
        bool isBandSubGHz      = bandFrequency < GHz ? true : false;
        if (currentAntenna == bsp::cellular::antenna::highBand) {
            LOG_INFO("High band antenna.");
            if (isBandSubGHz) {
                LOG_INFO("Modem connected on sub GHz band.");
                state->set(antenna::State::signalCheck);
                return true;
            }
        }

        if (currentAntenna == bsp::cellular::antenna::lowBand) {
            LOG_INFO("Low band antenna.");
            if (!isBandSubGHz) {
                LOG_INFO("Modem connected on over GHz band.");
                state->set(antenna::State::signalCheck);
                return true;
            }
        }

        LOG_INFO("Band match.");
        std::string csq;
        uint32_t csqValue = 0;
        if (CellularServiceAPI::GetCSQ(this, csq)) {
            at::response::parseCSQ(csq, csqValue);
            lastCsq = csqValue;
        }
        state->set(antenna::State::idle);
        return true;
    }
    return false;
}

bool ServiceAntenna::idleStateHandler(void)
{
    return true;
}

bool ServiceAntenna::csqChangeStateHandler(void)
{
    std::string csq;
    uint32_t csqValue = 0;

    constexpr uint32_t notValidCsq = 99;
    if (CellularServiceAPI::GetCSQ(this, csq)) {
        at::response::parseCSQ(csq, csqValue);
        currentCsq = csqValue;
    }

    auto nextState = antenna::State::idle;
    if (lastCsq != currentCsq || currentCsq == notValidCsq) {
        LOG_INFO("CSQ change occurent, check connection.");
        nextState = antenna::State::connectionStatus;
    }
    if (currentCsq != notValidCsq) {
        lastCsq = currentCsq;
    }

    state->set(nextState);
    return true;
}

bool ServiceAntenna::lockedStateHandler(void)
{
    state->disableTimeout();
    return true;
}
