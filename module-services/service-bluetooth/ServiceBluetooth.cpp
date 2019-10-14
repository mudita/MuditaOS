#include "ServiceBluetooth.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "MessageType.hpp"
#include <log/log.hpp>

const char *ServiceBluetooth::serviceName = "ServiceBluetooth";

ServiceBluetooth::ServiceBluetooth() : sys::Service(serviceName) {
    LOG_INFO("[ServiceBluetooth] Initializing");
    testTimerID = CreateTimer(3000,true);
    ReloadTimer(testTimerID);
}

ServiceBluetooth::~ServiceBluetooth() {
    LOG_INFO("[ServiceBluetooth] Cleaning resources");
}

// Invoked when timer ticked
void ServiceBluetooth::TickHandler(uint32_t id) {
}

// This code is experimental:
// this means it is an init point of bluetooth feature handling
sys::ReturnCodes ServiceBluetooth::InitHandler() {
    LOG_ERROR("Bluetooth experimental!");
    worker=std::make_unique<BluetoothWorker>(this);
    // LOG_INFO("BluetoothWorker -> scan!\n");
    // worker->scan();

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBluetooth::DeinitHandler() {

    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceBluetooth::DataReceivedHandler(sys::DataMessage* msg,sys::ResponseMessage* resp) {
    std::shared_ptr<sys::ResponseMessage> responseMsg;
    return responseMsg;
}

sys::ReturnCodes ServiceBluetooth::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_ERROR("TODO");
    return sys::ReturnCodes::Success;
}
