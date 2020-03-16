#pragma once

#include "Service/Service.hpp"
#include "Bluetooth/BluetoothWorker.hpp"
#include <memory>

class ServiceBluetooth : public sys::Service
{

  public:
    ServiceBluetooth();
    ~ServiceBluetooth();

    virtual sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    void TickHandler(uint32_t id) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

  private:
    static const char *serviceName;
    uint32_t testTimerID;
    std::unique_ptr<BluetoothWorker> worker;
};
