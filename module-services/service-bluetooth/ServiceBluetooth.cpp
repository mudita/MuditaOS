#include "ServiceBluetooth.hpp"
#include "Service/Service.hpp"
#include "Service/Message.hpp"
#include "MessageType.hpp"

#include "module-bluetooth/BT/BluetoothWorker.hpp"

const char *ServiceBluetooth::serviceName = "ServiceBluetooth";


ServiceBluetooth::ServiceBluetooth() : sys::Service(serviceName) {
    LOG_INFO("[ServiceBluetooth] Initializing");
    testTimerID = CreateTimer(3000,true);
    ReloadTimer(testTimerID);
    //// TODO TESTING
    BluetoothWorker();
}

ServiceBluetooth::~ServiceBluetooth() {
    LOG_INFO("[ServiceBluetooth] Cleaning resources");
}

// Invoked when timer ticked
void ServiceBluetooth::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ServiceBluetooth::InitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBluetooth::DeinitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBluetooth::WakeUpHandler() {
    return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceBluetooth::SleepHandler() {
    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceBluetooth::DataReceivedHandler(sys::DataMessage *msgl) {
    std::shared_ptr<sys::ResponseMessage> responseMsg;
    switch (static_cast<MessageType >(msgl->messageType)) {
        default:
            break;
    }
    return responseMsg;
}
