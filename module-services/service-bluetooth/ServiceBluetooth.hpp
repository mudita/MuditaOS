#pragma once

#include "Service/Service.hpp"
#include "module-bluetooth/BT/BluetoothWorker.hpp"

class ServiceBluetooth: public sys::Service  {

public:
    ServiceBluetooth();
    ~ServiceBluetooth();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    void TickHandler(uint32_t id) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes WakeUpHandler() override;
    sys::ReturnCodes SleepHandler() override;

private:
    static const char* serviceName;
    uint32_t testTimerID;
    BluetoothWorker *worker;
};
