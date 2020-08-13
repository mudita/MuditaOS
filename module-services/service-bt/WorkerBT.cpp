#include "WorkerBT.hpp"
#include "fsl_lpuart.h"

bool WorkerBT::handleMessage(uint32_t queueID)
{

    QueueHandle_t queue = queues[queueID];

    std::string qname = queueNameMap[queue];
    // LOG_INFO("[ServiceDesktop:Worker] Received data from queue: %s", qname.c_str());

    static std::string receiveMsg;
    // static std::string *sendMsg;

    if (qname == SERVICE_QUEUE_NAME) {
        LOG_ERROR("[ServiceDesktop:Worker] Service Queue invoked but not implemented!");
    }

    if (qname == RECEIVE_QUEUE_BUFFER_NAME) {
        if (xQueueReceive(queue, &receiveMsg, 0) != pdTRUE) {
            return false;
        }
        printf("data:\n");
        for (unsigned int i = 0; i < receiveMsg.length(); ++i)
        {
            printf("0x%X ", *(receiveMsg.c_str()+i));
        }
        printf("\n");

//        bt = bsp::BlueKitchen::getInstance();
//        bt->write_blocking(const_cast<char*>(receiveMsg.c_str()), receiveMsg.length());
        LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, (uint8_t*)(receiveMsg.c_str()), receiveMsg.length());
    }

    if (qname == UART_RECEIVE_QUEUE) {
        uint8_t evt;
        if (xQueueReceive(queue, &evt, 0) != pdTRUE) {
            return false;
        }
        if( evt == Bt::Message::EvtRecUnwanted ) {
        // if( evt == 
        char val;
        int ret = bt->in.pop(&val);
        if ( ret == 0 ) {
            auto send = new std::string(&val,1);
            printf("< 0x%X\n", val);
            /// TODO check if all is send because I don't fully believe it...
            bsp::usbCDCSend(send);
        } else {
            LOG_ERROR("ret: %d", ret);
        }
        } else {
            LOG_ERROR("event... %d", evt);
        }
    }

    return true;
}

bool WorkerBT::init(std::list<sys::WorkerQueueInfo> queues)
{
    LOG_DEBUG("init worker bt");
    Worker::init(queues);


    LOG_DEBUG("get bt instajce");
    bt = bsp::BlueKitchen::getInstance();
    bt->qHandle =  Worker::getQueueByName(WorkerBT::UART_RECEIVE_QUEUE);
    bt->open();
    // TODO here initialization and crap...

    LOG_DEBUG("start usb");
    auto queue = Worker::getQueueByName(WorkerBT::RECEIVE_QUEUE_BUFFER_NAME);
    if( queue == nullptr ) 
    {
        LOG_FATAL("No queue for USB");
    }
    // now start usb
    if ((bsp::usbCDCInit(queue) < 0)) {
        LOG_ERROR("won't start desktop service without serial port");
        return false;
    }
    LOG_DEBUG("WorkerBT initialized - usb Started");
    return true;
}

bool WorkerBT::deinit(void)
{
    Worker::deinit();
    return true;
}
