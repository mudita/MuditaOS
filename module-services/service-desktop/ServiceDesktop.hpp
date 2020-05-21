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

class ServiceDesktop : public sys::Service
{
  public:
    ServiceDesktop();
    ~ServiceDesktop();
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    std::unique_ptr<WorkerDesktop> &getWorker()
    {
        return (desktopWorker);
    }

    std::unique_ptr<UpdatePureOS> updateOS;
    std::unique_ptr<WorkerDesktop> desktopWorker;
    static const char *serviceName;
};
