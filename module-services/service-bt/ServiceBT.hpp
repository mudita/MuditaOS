#pragma once

#include "Service/Service.hpp"
#include <functional>
#include <memory>
#include "MessageType.hpp"
#include "WorkerBT.hpp"

class ServiceBT : public sys::Service
{
    std::unique_ptr<WorkerBT> worker;
  public:
    ServiceBT();
    ~ServiceBT() override = default;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    void TickHandler(uint32_t id) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;
};
