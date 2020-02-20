#include <string.h>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"

#include "parser/EndpointFsm.hpp"
#include "parser/Fsmlist.hpp"
#include "parser/SerialParserFsm.hpp"

#include "WorkerDesktop.hpp"

bool WorkerDesktop::handleMessage(uint32_t queueID)
{

    QueueHandle_t queue = queues[queueID];
    std::string qname = getQueueNameMap()[queue];
    LOG_INFO("[ServiceDesktop:Worker] Received data from queue: %s", qname.c_str());

    static std::string receive_msg;
    static std::string *send_msg;

    if (qname == "receiveQueueBuffer")
    {
        if (xQueueReceive(queue, &receive_msg, 0) != pdTRUE)
            return false;

        SerialParserFsm::msgChunk.assign(receive_msg.begin(), receive_msg.end());
        while (!SerialParserFsm::msgChunk.empty())
        {
            send_event(MessageDataEvt());
        }
    }

    if (qname == "sendQueueBuffer")
    {
        if (xQueueReceive(queue, &send_msg, 0) != pdTRUE)
            return false;

        bsp::desktopServiceSend(send_msg);
    }

    return true;
}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    Worker::init(queues);
    return true;
}

bool WorkerDesktop::deinit(void)
{
    Worker::deinit();
    return true;
}
