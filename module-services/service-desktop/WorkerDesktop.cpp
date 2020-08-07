#include "WorkerDesktop.hpp"
#include "EndpointHandler.hpp"
#include "MessageHandler.hpp"

bool WorkerDesktop::handleMessage(uint32_t queueID)
{

    QueueHandle_t queue = queues[queueID];

    std::string qname = queueNameMap[queue];
    LOG_INFO("[ServiceDesktop:Worker] Received data from queue: %s", qname.c_str());

    static std::string receiveMsg;
    static std::string *sendMsg;

    if (qname == SERVICE_QUEUE_NAME) {
        LOG_ERROR("[ServiceDesktop:Worker] Service Queue invoked but not implemented!");
    }

    if (qname == RECEIVE_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &receiveMsg, 0) != pdTRUE)
            return false;

        parser.processMessage(receiveMsg);
    }

    // TODO: Consider moving sendBuffer receive to bsp driver
    if (qname == SEND_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &sendMsg, 0) != pdTRUE)
            return false;

        bsp::usbCDCSend(sendMsg);
    }

    return true;
}

bool WorkerDesktop::init(std::list<sys::WorkerQueueInfo> queues)
{
    Worker::init(queues);

    if ((bsp::usbCDCInit(Worker::getQueueByName(WorkerDesktop::RECEIVE_QUEUE_BUFFER_NAME)) < 0)) {
        LOG_ERROR("won't start desktop service without serial port");
        return false;
    }

    ParserStateMachine::MessageHandler::sendQueue = Worker::getQueueByName(WorkerDesktop::SEND_QUEUE_BUFFER_NAME);
    EndpointHandler::initHelpers(ownerService);

    return true;
}

bool WorkerDesktop::deinit(void)
{
    Worker::deinit();
    return true;
}
