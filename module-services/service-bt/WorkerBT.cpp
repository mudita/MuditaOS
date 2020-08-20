#include "WorkerBT.hpp"
#include "fsl_lpuart.h"
#include "BtLogger.hpp"
#include "service-bt/BtInject.hpp"
#include <memory>

static bool we_can_send_usb = false;

WorkerBT::WorkerBT(sys::Service *ownerServicePtr) : sys::Worker(ownerServicePtr), ownerService(ownerServicePtr)
{
    logger = std::make_unique<BtLogger>("bt_log.txt");
}


WorkerBT::~WorkerBT()
{}

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
        logger->log(BtLogger::Event::In, (receiveMsg.c_str()), receiveMsg.length());

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
            if ( we_can_send_usb ) {
                auto ret = bsp::usbCDCSend(send);
                if ( ret != 0 ) 
                {
                    auto result = std::to_string(ret);
                    logger->log(BtLogger::Event::USB_Error, result.c_str(), result.length());
                }
            }
        } else {
            LOG_ERROR("ret: %d", ret);
        }
        } else {
            LOG_ERROR("event... %d", evt);
        }
    }

    return true;
}

// TODO
bool WorkerBT::initializer()
{
    inject = std::make_unique<BtInject>();
    return inject->parse_commands();
    //for (auto &cmd : bt_in) {
    //    LOG_DEBUG("CMD: ");
    //    for (unsigned char val : cmd) {
    //        printf("0x%X", val);
    //        LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, &val, 1);
    //    }
    //    printf("\n");
    //    ulTaskNotifyTake(true, 100); // dummy to wait instead check
    //}
}

bool WorkerBT::init(std::list<sys::WorkerQueueInfo> queues)
{
    LOG_DEBUG("init worker bt");
    Worker::init(queues);


    LOG_DEBUG("get bt instajce");
    bt = bsp::BlueKitchen::getInstance();
    bt->qHandle =  Worker::getQueueByName(WorkerBT::UART_RECEIVE_QUEUE);
    bt->open();

    initializer();

    LOG_DEBUG("start usb");
    auto queue = Worker::getQueueByName(WorkerBT::RECEIVE_QUEUE_BUFFER_NAME);
    if( queue == nullptr ) 
    {
        LOG_FATAL("No queue for USB");
    }
    // now start usb
    if ((bsp::usbCDCInit(queue) < 0)) {
        std::string log = "won't start desktop service without serial port";
        LOG_ERROR("%s",log.c_str());
        logger->log(BtLogger::Event::USB_Error, log.c_str(), log.length());
        return false;
    }
    we_can_send_usb = true;
    LOG_DEBUG("WorkerBT initialized - usb Started");

    /// TODO TIMER FOR
    /// logger->timed_flush();
    return true;
}

bool WorkerBT::deinit(void)
{
    Worker::deinit();
    return true;
}
