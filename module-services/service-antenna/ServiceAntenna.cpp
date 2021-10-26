// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <AntennaMessage.hpp>
#include <AntennaServiceAPI.hpp>
#include <ServiceAntenna.hpp>
#include <ServiceState.hpp>

#include <at/response.hpp>
#include <EventStore.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <Timers/TimerFactory.hpp>
#include <projdefs.h>
#include <service-cellular/CellularMessage.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-cellular/State.hpp>
#include <ticks.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

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

inline constexpr auto antennaServiceStackSize = 1024 * 2;

ServiceAntenna::ServiceAntenna()
    : sys::Service(service::name::antenna, "", antennaServiceStackSize, sys::ServicePriority::Idle),
      state{std::make_unique<state::State<antenna::State>>(this)}, phoneModeObserver{
                                                                       std::make_unique<sys::phone_modes::Observer>()}
{
    LOG_INFO("[%s] Initializing", service::name::antenna);
    timer = sys::TimerFactory::createPeriodicTimer(
        this, "Antenna", std::chrono::seconds{5}, [this](sys::Timer & /*timer*/) {
            timer.stop();
            auto stateToSet = state->get();
            if (state->timeoutOccured(cpp_freertos::Ticks::GetTicks())) {
                LOG_WARN("State [ %s ] timeout occurred, setting [ %s ] state",
                         c_str(state->get()),
                         c_str(state->getTimeoutState()));
                stateToSet = state->getTimeoutState();
            }
            state->set(stateToSet);
        });
    bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);
    bus.channels.push_back(sys::BusChannel::AntennaNotifications);
    bus.channels.push_back(sys::BusChannel::PhoneModeChanges);

    phoneModeObserver->connect(this);
    phoneModeObserver->subscribe([this](sys::phone_modes::PhoneMode mode) {
        if (mode == sys::phone_modes::PhoneMode::Offline) {
            handleLockRequest(antenna::lockState::locked);
        }
        else {
            handleLockRequest(antenna::lockState::unlocked);
        }
    });
}

ServiceAntenna::~ServiceAntenna()
{
    LOG_INFO("[%s] Cleaning resources", service::name::antenna);
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
    registerMessageHandlers();
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceAntenna::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

void ServiceAntenna::ProcessCloseReason(sys::CloseReason closeReason)
{
    sendCloseReadyMessage(this);
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
        timer.start();
    }
    return ret;
}

bool ServiceAntenna::initStateHandler(void)
{
    LOG_INFO("State Init");
    bsp::cellular::antenna antenna;
    if (phoneModeObserver->isInMode(sys::phone_modes::PhoneMode::Offline)) {
        auto message =
            std::make_shared<AntennaLockRequestMessage>(MessageType::AntennaLockService, antenna::lockState::locked);
        bus.sendUnicast(std::move(message), service::name::antenna);
        return true;
    }

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
    if (currentCsq <= antenna::signalTreshold) {
        LOG_INFO("Signal strength below the threshold (CSQ = %" PRIu32 ", Threshold = %" PRIu32 "). Switch antenna",
                 currentCsq,
                 antenna::signalTreshold);
        state->set(antenna::State::switchAntenna);
        return true;
    }
    lastCsq = currentCsq;
    state->set(antenna::State::idle);
    return true;
}
bool ServiceAntenna::bandCheckStateHandler(void)
{
    std::string cellularResponse;
    if (CellularServiceAPI::GetQNWINFO(this, cellularResponse)) {
        std::string qnwinfo;
        at::response::parseQNWINFO(cellularResponse, qnwinfo);
        LOG_INFO("QNWINFO: %s", qnwinfo.c_str());
        const auto bandFrequency = at::response::qnwinfo::parseNetworkFrequency(qnwinfo);
        LOG_INFO("Band frequency: %" PRIu32, bandFrequency);
        constexpr uint32_t GHz  = 1000;
        const bool isBandSubGHz = bandFrequency < GHz;
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
    constexpr uint32_t notValidCsq = 99;

    auto nextState = antenna::State::idle;
    if (lastCsq != currentCsq || currentCsq == notValidCsq) {
        LOG_INFO("CSQ change occurrence, check connection.");
        nextState = antenna::State::connectionStatus;
    }
    if (currentCsq != notValidCsq) {
        lastCsq = currentCsq;
    }
    LOG_INFO("CSQ value:  %" PRIu32, currentCsq);

    state->set(nextState);
    return true;
}

bool ServiceAntenna::lockedStateHandler(void)
{
    state->disableTimeout();
    return true;
}
void ServiceAntenna::registerMessageHandlers()
{
    connect(typeid(antenna::message::InvalidCsqNotification), [&](sys::Message *request) -> sys::MessagePointer {
        if (state->get() == antenna::State::idle) {
            state->set(antenna::State::connectionStatus);
        }
        return sys::MessageNone{};
    });

    connect(typeid(cellular::msg::notification::SimReady), [this](sys::Message *) {
        state->set(antenna::State::init);
        return sys::MessageNone{};
    });

    connect(typeid(antenna::message::InvalidCsqNotification), [&](sys::Message *request) -> sys::MessagePointer {
        if (state->get() == antenna::State::idle) {
            state->set(antenna::State::connectionStatus);
        }
        return sys::MessageNone{};
    });

    connect(typeid(CellularSignalStrengthUpdateNotification), [&](sys::Message *request) -> sys::MessagePointer {
        currentCsq = Store::GSM::get()->getSignalStrength().rssi;
        if (state->get() == antenna::State::idle) {
            state->set(antenna::State::csqChange);
        }
        return sys::MessageNone{};
    });

    connect(typeid(CellularIncominCallMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto message =
            std::make_shared<AntennaLockRequestMessage>(MessageType::AntennaLockService, antenna::lockState::locked);
        bus.sendUnicast(std::move(message), service::name::antenna);
        return sys::MessageNone{};
    });

    connect(typeid(CellularRingingMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto message =
            std::make_shared<AntennaLockRequestMessage>(MessageType::AntennaLockService, antenna::lockState::locked);
        bus.sendUnicast(std::move(message), service::name::antenna);
        return sys::MessageNone{};
    });

    connect(typeid(CellularHangupCallMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto message =
            std::make_shared<AntennaLockRequestMessage>(MessageType::AntennaLockService, antenna::lockState::unlocked);
        bus.sendUnicast(std::move(message), service::name::antenna);
        return sys::MessageNone{};
    });

    connect(typeid(CellularCallAbortedNotification), [&](sys::Message *request) -> sys::MessagePointer {
        auto message =
            std::make_shared<AntennaLockRequestMessage>(MessageType::AntennaLockService, antenna::lockState::unlocked);
        bus.sendUnicast(std::move(message), service::name::antenna);
        return sys::MessageNone{};
    });
}
