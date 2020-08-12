#include "ServiceBT.hpp"

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
{}

sys::ReturnCodes ServiceBT::InitHandler()
{
    LOG_DEBUG("Init!");
    worker = std::make_unique<WorkerBT>(this);
    worker->init( {{worker->RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string), 100}});
    worker->run();
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
