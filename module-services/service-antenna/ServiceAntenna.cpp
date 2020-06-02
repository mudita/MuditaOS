
#include "ServiceAntenna.hpp"
#include "log/log.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <service-cellular/messages/CellularMessage.hpp>
#include <module-utils/state/ServiceState.hpp>

#include <cassert>

const char *ServiceAntenna::serviceName = "ServiceAntenna";

namespace antenna
{
    const char *c_str(antenna::State state)
    {
        switch (state) {
        case antenna::State::none:
            return "none";
            break;
        case antenna::State::init:
            return "init";
            break;
        default:
            return "";
            break;
        }
    }
} // namespace antenna

ServiceAntenna::ServiceAntenna() : sys::Service(serviceName)
{
    LOG_INFO("[%s] Initializing", serviceName);
    state = new utils::state::State<antenna::State>(this);
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
    vTaskDelay(10000);
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

bool ServiceAntenna::HandleStateChange(antenna::State state)
{
    bool ret = false;
    switch (state) {
    case antenna::State::none:
        NoneStateHandler();
        ret = true;
        break;
    case antenna::State::init:
        InitStateHandler();
        ret = true;
        break;
    default:
        break;
    }
    return ret;
}
bool ServiceAntenna::InitStateHandler(void)
{
    LOG_INFO("State Init");
    state->set(antenna::State::none);
    return true;
}

bool ServiceAntenna::NoneStateHandler(void)
{
    LOG_INFO("State Init");
    state->set(antenna::State::none);
    return true;
}
