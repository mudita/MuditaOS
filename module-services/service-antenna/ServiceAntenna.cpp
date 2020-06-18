
#include "ServiceAntenna.hpp"
#include "log/log.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <service-cellular/messages/CellularMessage.hpp>
#include <module-utils/state/ServiceState.hpp>
#include <at/response.hpp>

#include <cassert>

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
        case antenna::State::highBand:
            return "highBand";
        case antenna::State::lowBand:
            return "lowBand";
        case antenna::State::signalCheck:
            return "signalCheck";
        case antenna::State::bandCheck:
            return "bandCheck";
        case antenna::State::idle:
            return "idle";
        default:
            return "";
            break;
        }
    }
} // namespace antenna

ServiceAntenna::ServiceAntenna() : sys::Service(serviceName)
{
    LOG_INFO("[%s] Initializing", serviceName);

    timerID = CreateTimer(5000, true);

    state = new utils::state::State<antenna::State>(this);

    busChannels.push_back(sys::BusChannels::AntennaNotifications);
}

ServiceAntenna::~ServiceAntenna()
{
    LOG_INFO("[%s] Cleaning resources", serviceName);
}

// Invoked upon receiving data message
sys::Message_t ServiceAntenna::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    bool handled = false;

    switch (msgl->messageType) {
    case MessageType::StateChange:
        LOG_INFO("STATE MESSAGE");
        HandleStateChange(state->get());
        break;
    case MessageType::AntennaChanged: {
        bsp::cellular::antenna antenna;
        if (CellularServiceAPI::GetAntenna(this, antenna)) {
            currentAntenna = antenna;
            if (state->get() == antenna::State::switchAntenna) {
                LOG_INFO("Antena switched.");
                state->set(antenna::State::connectionStatus);
            }
        }
        handled = true;
        break;
    }
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
    //    stateMachine->addState(antenna::State::init, InitStateHandler);
    vTaskDelay(30000);
    state->set(antenna::State::init);

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

void ServiceAntenna::TickHandler(uint32_t id)
{
    stopTimer(timerID);
    state->set(state->get());
}

void ServiceAntenna::storeCurrentState(void)
{

    std::string cellularResponse;
    if (CellularServiceAPI::GetCSQ(this, cellularResponse)) {
        uint32_t csq = 0;
        if (at::response::parseCSQ(cellularResponse, csq)) {
            storedCsq = csq;
        }
    }

    if (CellularServiceAPI::GetCREG(this, cellularResponse)) {
        uint32_t creg = 0;
        if (at::response::parseCREG(cellularResponse, creg)) {
            storedRegisterd = at::response::creg::isRegistered(creg);
        }
    }
}

bool ServiceAntenna::HandleStateChange(antenna::State state)
{
    bool ret = false;
    switch (state) {
    case antenna::State::none:
        noneStateHandler();
        ret = true;
        break;
    case antenna::State::init:
        initStateHandler();
        ret = true;
        break;
    case antenna::State::connectionStatus:
        ret = connectionStatusStateHandler();
        break;
    case antenna::State::switchAntenna:
        ret = switchAntennaStateHandler();
        break;
    case antenna::State::highBand:
        ret = highBandStateHandler();
        break;
    case antenna::State::lowBand:
        ret = lowBandStateHandler();
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
    default:
        break;
    }
    if (!ret) {
        LOG_WARN("State [ %s ] not handled. Reloading timer.", c_str(state));
        ReloadTimer(timerID);
    }
    return ret;
}

bool ServiceAntenna::initStateHandler(void)
{
    LOG_INFO("State Init");
    bsp::cellular::antenna antenna;
    if (CellularServiceAPI::GetAntenna(this, antenna)) {
        currentAntenna = antenna;

        state->set(antenna::State::connectionStatus);
        return true;
    }
    return false;
}

bool ServiceAntenna::noneStateHandler(void)
{
    return false;
}

bool ServiceAntenna::connectionStatusStateHandler(void)
{
    std::string cellularResponse;
    CellularServiceAPI::GetCREG(this, cellularResponse);
    uint32_t cregValue;
    at::response::parseCREG(cellularResponse, cregValue);
    if (at::response::creg::isRegistered(cregValue)) {
        LOG_INFO("Modem connected.");
        state->set(antenna::State::bandCheck);
        return true;
    }
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
bool ServiceAntenna::highBandStateHandler(void)
{
    return true;
}
bool ServiceAntenna::lowBandStateHandler(void)
{
    return true;
}
bool ServiceAntenna::signalCheckStateHandler(void)
{

    std::string csq;
    if (CellularServiceAPI::GetCSQ(this, csq)) {
        uint32_t csqValue = 0;
        at::response::parseCSQ(csq, csqValue);
        if (csqValue <= antenna::signalTreshold) {
            LOG_INFO("Signal strength below treshold. Switch antenna");
            state->set(antenna::State::switchAntenna);
            return true;
        }
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

        auto bandFrequency = at::response::qnwinfo::parseNetworkFrequency(band);

        LOG_INFO("Band frequency: %lu", bandFrequency);
        constexpr uint32_t GHz = 1000;
        bool isBandSubGHz      = bandFrequency < GHz ? true : false;
        if (isBandSubGHz && currentAntenna == bsp::cellular::antenna::highBand) {
            LOG_INFO("sub GHz band.");
            state->set(antenna::State::signalCheck);
            return true;
        }
        LOG_INFO("over GHz band.");
        state->set(antenna::State::idle);
        return true;
    }
    return false;
}

bool ServiceAntenna::idleStateHandler(void)
{

    return false;
}
