#pragma once

#include "WorkerDesktop.hpp"
#include "UpdatePureOS.h"

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
        UpdateOsMessage(MessageType messageType, const std::string updateFilePath, const uint32_t requestUUID)
            : sys::DataMessage(messageType), updateFile(updateFilePath), uuid(requestUUID){};
        UpdateOsMessage(MessageType type = MessageType::UpdateOS) : sys::DataMessage(type)
        {}
        virtual ~UpdateOsMessage(){};

        std::string updateFile;
        uint32_t uuid;
    };
}; // namespace sdesktop

class ServiceDesktop : public sys::Service
{
  public:
    ServiceDesktop();
    ~ServiceDesktop();
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

    std::unique_ptr<UpdatePureOS> updateOS;
    std::unique_ptr<WorkerDesktop> desktopWorker;
};
