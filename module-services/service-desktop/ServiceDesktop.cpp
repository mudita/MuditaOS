#include "ServiceDesktop.hpp"
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "json/json11.hpp"
#include <errno.h>
#include <log/log.hpp>
#include <string.h>

#include "parser/Fsmlist.hpp"
#include "parser/SerialParserFsm.hpp"
#include "parser/EndpointFsm.hpp"

const char *ServiceDesktop::serviceName = "ServiceDesktop";

// Temporary solution to queue passing to bsp driver mock up
xQueueHandle USBReceiveQueue;
xQueueHandle USBSendQueue;

ServiceDesktop::ServiceDesktop() : sys::Service(serviceName), taskHandleReceive(nullptr), taskHandleSend(nullptr), ptyDescriptor(-1)
{
    LOG_INFO("[ServiceDesktop] Initializing");
}

ServiceDesktop::~ServiceDesktop()
{

    LOG_INFO("[ServiceDesktop] Cleaning resources");
    if (DesktopWorker != nullptr)
    {
        DesktopWorker->deinit();
    }
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{

    DesktopWorker = std::make_unique<WorkerDesktop>(this);
    DesktopWorker->init({{"receiveQueueBuffer", sizeof(std::string), 10}, {"sendQueueBuffer", sizeof(std::string), 10}});
    DesktopWorker->run();

    USBReceiveQueue = DesktopWorker->getQueues().at(1);
    USBSendQueue = DesktopWorker->getQueues().at(2);

    //    const char *name = pcQueueGetQueueName(USBReciveQueue);
    //    const char *name2 = pcQueueGetQueueName(USBSendQueue);
    //
    //    std::string name3 = DesktopWorker->getQueueNameMap()[DesktopWorker->getQueues().at(1)];

    if ((ptyDescriptor = desktopServiceInit()) < 0)
    {
        LOG_ERROR("won't start desktop service without serial port");
        return (sys::ReturnCodes::Failure);
    }

    fsm_list::start();

    return (sys::ReturnCodes::Success);
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDesktop::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceDesktop::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    return std::make_shared<sys::ResponseMessage>();
}

void ServiceDesktop::dataReceived(const uint8_t *data, const ssize_t dataLen)
{
    SerialParserFsm::msgChunk.assign(data, data + dataLen);
    send_event(MessageDataEvt());

}

int ServiceDesktop::getPtyDescriptor()
{
    return (ptyDescriptor);
}
