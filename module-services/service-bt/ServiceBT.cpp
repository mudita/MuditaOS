#include "ServiceBT.hpp"
#include "FreeRTOS.h"
#include "WorkerCmds.hpp"
#include "BtInject.hpp"
#include "bsp/rtc/rtc.hpp"

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
        auto expecting = *(std::next(inject->getResponses().begin(), pos));
        bt_write(cmd);

        auto ret = bt_read(expecting.size(), 250);
        if (expecting != ret) {
            LOG_DEBUG("response missmatch on cmd no: %d", pos);
        }

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
        LOG_ERROR("BT waited for data timed out got %u of: %"PRIu32, cmd->data.size(), expected_count);
    }

    auto data = cmd->data;
    to_send.cleanup();
    return data;
}
