#pragma once

#include "Service/Service.hpp"
#include <functional>
#include "Audio/Audio.hpp"
#include "MessageType.hpp"

class ServiceBT : public sys::Service
{

  public:
    ServiceBT();
    ~ServiceBT() override = default;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    void TickHandler(uint32_t id) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;
};
