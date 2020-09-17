#pragma once

#include "WorkerDesktop.hpp"
#include "module-services/service-desktop/endpoints/update/UpdatePureOS.hpp"

namespace service::name
{
    const inline std::string service_desktop = "ServiceDesktop";
};

namespace sdesktop
{
    const inline int service_stack         = 8192;
    const inline int cdc_queue_len         = 10;
    const inline int cdc_queue_object_size = 10;
    class UpdateOsMessage : public sys::DataMessage
    {
      public:
        UpdateOsMessage(const std::string updateFilePath, const uint32_t requestUUID)
            : sys::DataMessage(MessageType::UpdateOS), updateFile(updateFilePath), uuid(requestUUID){};
        UpdateOsMessage() : sys::DataMessage(MessageType::UpdateOS)
        {}
        ~UpdateOsMessage() override = default;

        std::string updateFile;
        uint32_t uuid;
    };
}; // namespace sdesktop

class ServiceDesktop : public sys::Service
{
  public:
    ServiceDesktop();
    ~ServiceDesktop() override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

    std::unique_ptr<UpdatePureOS> updateOS;
    std::unique_ptr<WorkerDesktop> desktopWorker;
};
