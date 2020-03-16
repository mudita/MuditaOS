#include "ServiceDesktop.hpp"

const char *ServiceDesktop::serviceName = "ServiceDesktop";

ServiceDesktop::ServiceDesktop() : sys::Service(serviceName)
{
    LOG_INFO("[ServiceDesktop] Initializing");
}

ServiceDesktop::~ServiceDesktop()
{

    LOG_INFO("[ServiceDesktop] Cleaning resources");
    if (DesktopWorker != nullptr) {
        DesktopWorker->deinit();
    }
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{

    DesktopWorker = std::make_unique<WorkerDesktop>(this);
    DesktopWorker->init({{DesktopWorker->RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string), 1},
                         {DesktopWorker->SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), 10}});
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
