#include "ServiceDesktop.hpp"
#include <random>
#include <../module-vfs/vfs.hpp>
#include <time/time_conversion.hpp>

const char *ServiceDesktop::serviceName = "ServiceDesktop";

ServiceDesktop::ServiceDesktop() : sys::Service(serviceName)
{
    LOG_INFO("[ServiceDesktop] Initializing");
}

ServiceDesktop::~ServiceDesktop()
{
    LOG_INFO("[ServiceDesktop] Cleaning resources");
#ifdef TARGET_RT1051
    if (desktopWorker != nullptr) {
        desktopWorker->deinit();
    }
#endif
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
#ifdef TARGET_RT1051
    desktopWorker = std::make_unique<WorkerDesktop>(this);
    desktopWorker->init({{desktopWorker->RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string), 1},
                         {desktopWorker->SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), 10}});
    desktopWorker->run();

    updateOS = std::make_unique<UpdatePureOS>(this);
#endif
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