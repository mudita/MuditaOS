#pragma once

#include "WorkerDesktop.hpp"
#include "UpdatePureOS.h"

#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBMessage.hpp"
#include "vfs.hpp"

#include <FreeRTOS.h>
#include <task.h>
#include <memory>
#include <string.h>
#include <sstream>

namespace sdesktop
{
    class UpdateOsMessage : public sys::DataMessage
    {
      public:
        UpdateOsMessage(MessageType messageType, const std::string updateFilePath, const uint32_t requestUUID)
            : sys::DataMessage(messageType), updateFile(updateFilePath), uuid(requestUUID){};
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
    static const char *serviceName;
};
