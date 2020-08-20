#include "WorkerBT.hpp"
#include "fsl_lpuart.h"
#include "BtLogger.hpp"
#include "service-bt/BtInject.hpp"
#include <memory>
#include <ios>
#include "WorkerCmds.hpp"

static bool we_can_send_usb = false;
/// TODO blocking read like in service cellular :|
/// we know how much to expect or else - just wait max 100ms

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

    static std::string receiveMsg;

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
            logger->log_out_byte(val);
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

    if ( qname == BT_COMMANDS ) 
    {
        BtCmd cmd;
        if (xQueueReceive(queue, &cmd, 0) != pdTRUE) {
            return false;
        }

        if( cmd.cmd == BtCmd::Cmd::Init) 
        {
            // TODO ths will stuck uart receive ----
            initializer();
        }

        if( cmd.cmd == BtCmd::Cmd::TimerPoll) 
        {
            logger->timed_flush();
        }

    }

    return true;
}

// TODO
bool WorkerBT::initializer()
{
    inject = std::make_unique<BtInject>();
    if (inject->parse_commands() == false) {
        return false;
    }
    for ( auto &cmd : inject->getCommands() ) 
    // auto cmd = inject->getCommands().front();
    {
        for ( uint8_t ch : cmd ) {
            LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, &ch, 1);
        }

        auto cmd_txt = [&]()->std::string{
                        std::stringstream ss;
                        for ( auto el : cmd ) 
                        {
                            ss << " 0x" << std::hex << (int)el;
                        }
                        return ss.str();
                        }();

        LOG_DEBUG("cmd: %s", cmd_txt.c_str());
        logger->log(BtLogger::Event::In, (cmd_txt.c_str()), cmd_txt.length());

        ulTaskNotifyTake(true, 100); // dummy to wait instead check
    }
    LOG_DEBUG("sent: %d", inject->getCommands().size());
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

    return true;
}

bool WorkerBT::deinit(void)
{
    Worker::deinit();
    return true;
}
