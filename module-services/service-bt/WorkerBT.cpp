#include "WorkerBT.hpp"
#include "fsl_lpuart.h"
#include "BtLogger.hpp"
#include "service-bt/BtInject.hpp"
#include <memory>
#include <ios>
#include "WorkerCmds.hpp"

static bool we_can_send_usb = false;

struct BtInReceive {
    static std::list<char> rec_data; // block it when we are not in read/write mode <-- in pass through
    TaskHandle_t read_handle = nullptr;  // handle to notify when we end read
    std::list<char> *rec_final_buffer = nullptr; // block of data we put data when we are requested for it
    uint32_t to_read_count = 0; // num of bytes to read

    void clear_rec_data() 
    {
        rec_data = {};
    }

    // add data and if we are done - notify
    void add_rec_data(char val) 
    {
        rec_data.push_back(val);
        if_read_done_notify();
    }

    void read_done_notify()
    {
        BaseType_t hp = pdFALSE;
        if (read_handle != nullptr) {
            vTaskNotifyGiveFromISR(read_handle, &hp);
            read_handle = nullptr;
        }
        to_read_count = 0;
        rec_final_buffer = nullptr;
    }

    void fill_read_data()
    {
        if ( rec_final_buffer != nullptr ) {
            for ( auto val : rec_data ) 
            {
                rec_final_buffer->push_back(val);
            }
        }
        rec_data.clear();
    }

    auto is_read_done() -> bool 
    {
        return rec_data.size() >= to_read_count;
    }

    void if_read_done_notify()
    {
        fill_read_data();
        if ( is_read_done() )
        {
            read_done_notify();
        }
    }

    void set_to_read_count(uint32_t to_read ) 
    {
        to_read_count = to_read;
    }

    void set_thread_notify(TaskHandle_t task)
    {
        read_handle = task;
    }

    void set_out_buffer(std::list<char> *buf)
    {
        rec_final_buffer = buf;
    }

} bt_receive;

std::list<char> BtInReceive::rec_data;

/// TODO blocking read like in service cellular :|
/// we know how much to expect or else - just wait max 100ms

WorkerBT::WorkerBT(sys::Service *ownerServicePtr) : sys::Worker(ownerServicePtr), ownerService(ownerServicePtr)
{
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
        BtLogger::get().log(BtLogger::Event::In, (receiveMsg.c_str()), receiveMsg.length());

        LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, (uint8_t*)(receiveMsg.c_str()), receiveMsg.length());
    }

    if (qname == UART_RECEIVE_QUEUE) {
        uint8_t evt;
        if (xQueueReceive(queue, &evt, 0) != pdTRUE) {
            return false;
        }
        // if waiting - then pop as much as we can in read
        if( evt == Bt::Message::EvtRecUnwanted ) {
        char val;
        int ret = bt->in.pop(&val);
        if ( ret == 0 ) {
            bt_receive.add_rec_data(val);
            auto send = new std::string(&val,1);
            BtLogger::get().log_out_byte(val);
            if ( we_can_send_usb ) {
                auto ret = bsp::usbCDCSend(send);
                if ( ret != 0 ) 
                {
                    auto result = std::to_string(ret);
                    BtLogger::get().log(BtLogger::Event::USB_Error, result.c_str(), result.length());
                }
            }
            if ( expecting_bytes > 0 ) {
                expecting_bytes--;
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

        switch ( cmd.cmd ) {

        case BtCmd::Cmd::Init: {
        } break;

        case BtCmd::Cmd::TimerPoll: {
            BtLogger::get().timed_flush();
        } break;

        case BtCmd::Cmd::Write: {

            bt_receive.clear_rec_data(); // empty local buffer

            auto data = dynamic_cast<BtWrite*>(cmd.ptr);
            assert(data);

            auto cmd_txt = [&]()->std::string{
                            std::stringstream ss;
                            for ( auto el : data->command ) 
                            {
                                ss << " 0x" << std::hex << (int)el;
                            }
                            return ss.str();
                            }();

            BtLogger::get().log(BtLogger::Event::In, (cmd_txt.c_str()), cmd_txt.length());

            for (uint8_t ch : data->command) {
                LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, &ch, 1);
            }
            cmd.cleanup();
            LOG_DEBUG("> In: %s", cmd_txt.c_str());
        } break;

        case BtCmd::Cmd::Read: {
            auto data = dynamic_cast<BtRead*>(cmd.ptr);
            bt_receive.set_to_read_count(data->to_read_count);  // set how many bytes to read
            bt_receive.set_thread_notify(data->read_handle);
            bt_receive.set_out_buffer(&(data->data));           // set buffer to set data when rec is done ( TODO ) 
            bt_receive.if_read_done_notify();                   // if we somehow received enough data
        }; break;
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
        BtLogger::get().log(BtLogger::Event::USB_Error, log.c_str(), log.length());
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
