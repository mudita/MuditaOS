#include "ServiceDesktop.hpp"

const char *ServiceDesktop::serviceName = "ServiceDesktop";

ServiceDesktop::ServiceDesktop() : sys::Service(serviceName), taskHandleReceive(nullptr), taskHandleSend(nullptr)
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
    DesktopWorker->init({{"receiveQueueBuffer", sizeof(std::string), 1}, {"sendQueueBuffer", sizeof(std::string *), 10}});
    DesktopWorker->run();

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
