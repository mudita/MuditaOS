#pragma once

#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"

#define SERIAL_TRANSMISSION_START 0x02
#define SERIAL_TRANSMISSION_END 0x03
#define SERIAL_SHELL_START 0x33
#define SERIAL_BAUDRATE 115200
#define SERIAL_BUFFER_LEN 512 // this matches the buffer length in rt1051 cdc implementaion

#if defined(TARGET_RT1051)
extern "C"
{
#include "rt1051/include/virtual_com.h"
}
#include "rt1051/Worker.hpp"
#else
#include "linux/Worker.hpp"
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
