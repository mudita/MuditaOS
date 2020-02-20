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
#include "parser/Fsmlist.hpp"

#if defined(TARGET_RT1051)
extern "C"
{
#include "rt1051/include/virtual_com.h"
}
#include "rt1051/usb_cdc.hpp"
#else
#include "linux/usb_cdc.hpp"
#endif

class WorkerDesktop : public sys::Worker
{
  private:
  public:
    WorkerDesktop(sys::Service *service) : sys::Worker(service){};
    /**
     * This function is responsible for creating all queues provided in the constructor.
     * When all queues are created this method creates set of queues.
     */
    virtual bool init(std::list<sys::WorkerQueueInfo> queues) override;
    virtual bool deinit() override;

    /**
     * This method is called from thread when new message arrives in queue.
     * @param queueID Index of the queue in the queues vector.
     */
    bool handleMessage(uint32_t queueID) override final;
};
