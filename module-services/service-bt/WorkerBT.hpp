
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

class WorkerBT : public sys::Worker
{
  private:
      bsp::BlueKitchen *bt = nullptr;
  public:
    const std::string RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    // const std::string SEND_QUEUE_BUFFER_NAME    = "sendQueueBuffer";

    WorkerBT(sys::Service *ownerServicePtr)
        : sys::Worker(ownerServicePtr), ownerService(ownerServicePtr) {
        }
    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;
    bool handleMessage(uint32_t queueID) override final;
    sys::Service *ownerService = nullptr;
};
