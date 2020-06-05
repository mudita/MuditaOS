#include "ServiceDesktop.hpp"
#include <time/time_conversion.hpp>
#include <vfs.hpp>
#include <random>

const char *ServiceDesktop::serviceName = "ServiceDesktop";

ServiceDesktop::ServiceDesktop() : sys::Service(serviceName, "", sdesktop::service_stack)
{
    LOG_INFO("[ServiceDesktop] Initializing");

    updateOS = std::make_unique<UpdatePureOS>(this);
}

ServiceDesktop::~ServiceDesktop()
{
    LOG_INFO("[ServiceDesktop] Cleaning resources");
    if (desktopWorker != nullptr) {
        desktopWorker->deinit();
    }
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
    desktopWorker = std::make_unique<WorkerDesktop>(this);
    desktopWorker->init(
        {{desktopWorker->RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string), sdesktop::cdc_queue_len},
         {desktopWorker->SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_object_size}});
    desktopWorker->run();

    connect(sdesktop::UpdateOsMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        sdesktop::UpdateOsMessage *updateOs = static_cast<sdesktop::UpdateOsMessage *>(msg);
        if (updateOs) {
            LOG_DEBUG("ServiceDesktop::DataReceivedHandler file:%s uuuid:%" PRIu32 "",
                      updateOs->updateFile.c_str(),
                      updateOs->uuid);
            updateOS->runUpdate(updateOs->updateFile);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

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
