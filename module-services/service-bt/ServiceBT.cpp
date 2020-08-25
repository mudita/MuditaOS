#include "ServiceBT.hpp"
#include "FreeRTOS.h"
#include "WorkerCmds.hpp"
#include "BtInject.hpp"
#include "bsp/rtc/rtc.hpp"
#include "BtLogger.hpp"
#include "service-bt/messages/BtInject.hpp"
#include <string>

BtInject::Command set_power = {0x01,0x84,0xfd,0x0c,0x00,0x03,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

ServiceBT::ServiceBT() : sys::Service("ServiceBT", "", 4096 * 2, sys::ServicePriority::Idle)
{
    LOG_DEBUG("service bt initialized");
}

sys::Message_t ServiceBT::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;
    auto msgType = static_cast<int>(msgl->messageType);
    LOG_DEBUG("msgType %d", msgType);
    return responseMsg;
}

void ServiceBT::TickHandler(uint32_t id)
{
    if (id == bt_timer) {
        BtCmd cmd;
        cmd.cmd = BtCmd::Cmd::TimerPoll;
        xQueueSend(worker->getQueueByName(worker->BT_COMMANDS), &cmd, portMAX_DELAY);
    }
}

sys::ReturnCodes ServiceBT::InitHandler()
{
    LOG_DEBUG("Init!");
    worker = std::make_unique<WorkerBT>(this);
    worker->init({{worker->RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string), 100},
                  {worker->UART_RECEIVE_QUEUE, sizeof(Bt::Message), 10},
                  {worker->BT_COMMANDS, sizeof(BtCmd), 3}});
    worker->run();

    BtCmd cmd;
    cmd.cmd = BtCmd::Cmd::Init;
    xQueueSend(worker->getQueueByName(worker->BT_COMMANDS), &cmd, portMAX_DELAY);

    initializer();

    bt_timer = CreateTimer(200, true);
    ReloadTimer(bt_timer);

    connect(BtOnOff(), [&](sys::DataMessage *, sys::ResponseMessage *) {
        LOG_INFO("service bt received setting value!");
        bt_write(set_power);
        return sys::Message_t();
    });

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBT::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceBT::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("PowerModeHandler: %s", c_str(mode));
    return sys::ReturnCodes::Success;
}

bool ServiceBT::initializer()
{
    auto inject = std::make_unique<BtInject>();
    if (inject->parse_commands() == false) {
        return false;
    }
    auto pos = 0;
    LOG_DEBUG("send: %u commands", inject->getCommands().size());
    for (auto &cmd : inject->getCommands()) {
        // auto expecting = *(std::next(inject->getResponses().begin(), pos));
        bt_write(cmd);

        // sleep 250 ms
        {
            auto newtime = xTaskGetTickCount() + 250;
            while (true) {
                ulTaskNotifyTake(1, 10); // poll each 10ms
                if (xTaskGetTickCount() > newtime) {
                    break;
                }
            }
        }

//        auto ret = bt_read(expecting.size(), 250);
//        if (expecting != ret) {
//            LOG_DEBUG("response missmatch on cmd no: %d", pos);
//        }

        pos += 1;
    }
    LOG_DEBUG("sent: %d", inject->getCommands().size());
    return true;
}

bool ServiceBT::bt_write(const BtInject::Command &command)
{
    BtCmd to_send = {BtCmd::Cmd::Write, new BtWrite(command) };
    // ay we work on temp memory here...
    xQueueSend(worker->getQueueByName(worker->BT_COMMANDS), &to_send, portMAX_DELAY);
    return true;
}

BtInject::Command ServiceBT::bt_read(uint32_t expected_count, uint32_t timeout)
{
    auto cmd = new BtRead();
    cmd->read_handle = GetHandle();
    cmd->to_read_count = expected_count;
    BtCmd to_send = {BtCmd::Cmd::Read, cmd};
    xQueueSend(worker->getQueueByName(worker->BT_COMMANDS), &to_send, portMAX_DELAY);

    auto newtime = xTaskGetTickCount()+ timeout;
    while (expected_count >= cmd->data.size()) {
        ulTaskNotifyTake(1, 10); // poll each 10ms
        if( xTaskGetTickCount() > newtime )
        {
            cmd->timed_out = true;
            break;
        }
    }

    // log if request timed out
    if (cmd->timed_out == true) {
        std::string log = "data timeout " + std::to_string(cmd->data.size()) + " of: " + std::to_string(expected_count);
        LOG_ERROR("BT %s", log.c_str());
        BtLogger::get().log(BtLogger::Event::BtInfo, log.c_str(), log.length());
    }

    auto data = cmd->data;
    to_send.cleanup();
    return data;
}
