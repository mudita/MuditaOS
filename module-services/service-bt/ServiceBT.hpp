#pragma once

#include "Service/Service.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include "MessageType.hpp"
#include "WorkerBT.hpp"
#include "BtInject.hpp"

class ServiceBT : public sys::Service
{
    std::unique_ptr<WorkerBT> worker;

    uint32_t bt_timer = 0;

    bool initializer();
  public:
    ServiceBT();
    ~ServiceBT() override = default;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp = nullptr) override;
    void TickHandler(uint32_t id) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final;

    auto bt_write(const BtInject::Command &command) -> bool;
    BtInject::Command bt_read(uint32_t expected_count, uint32_t timeout);
};
