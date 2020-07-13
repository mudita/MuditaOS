#include "ServiceDesktop.hpp"
#include "common_data/EventStore.hpp"
#include "source/version.hpp"

ServiceDesktop::ServiceDesktop() : sys::Service(service::name::service_desktop, "", sdesktop::service_stack)
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
        sdesktop::UpdateOsMessage *updateOsMsg = dynamic_cast<sdesktop::UpdateOsMessage *>(msg);
        if (updateOsMsg != nullptr) {
            LOG_DEBUG("ServiceDesktop::DataReceivedHandler file:%s uuuid:%" PRIu32 "",
                      updateOsMsg->updateFile.c_str(),
                      updateOsMsg->uuid);

            if (updateOS->setUpdateFile(updateOsMsg->updateFile) == updateos::UpdateError::NoError)
                updateOS->runUpdate();
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    Store::OperatingSystem::modify().version.initFromMemory();
    LOG_DEBUG("OS: %s", Store::OperatingSystem::get().version.toJson().dump().c_str());

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
