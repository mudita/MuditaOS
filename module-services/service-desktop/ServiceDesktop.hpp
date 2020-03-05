#pragma once

#include "FreeRTOS.h"
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"
#include "Service/Worker.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBMessage.hpp"
#include "task.h"
#include "vfs.hpp"
#include <memory>
#include <string.h>

#include "WorkerDesktop.hpp"

class ServiceDesktop : public sys::Service
{

  private:
    std::unique_ptr<WorkerDesktop> DesktopWorker;
    static const char *serviceName;
    bool isOsUpdating;

  public:
    ServiceDesktop();
    ~ServiceDesktop();

    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

    sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

    void updateOs(const bool update = false) { isOsUpdating = update; }
};
