
#pragma once

#include <string.h>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "bsp/usb_cdc/usb_cdc.hpp"
#include "bsp/bluetooth/Bluetooth.hpp"

#include <memory>
class BtLogger;

class WorkerBT : public sys::Worker
{
  private:
      bsp::BlueKitchen *bt = nullptr;
      std::unique_ptr<BtLogger> logger;
      uint32_t expecting_bytes = 0;
  public:
    const std::string RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    const std::string UART_RECEIVE_QUEUE    = "sendQueueBuffer";
    const std::string BT_COMMANDS = "btCommands";

    WorkerBT(sys::Service *ownerServicePtr);
    virtual ~WorkerBT();
    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;
    bool handleMessage(uint32_t queueID) override final;
    sys::Service *ownerService = nullptr;
};
