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
#include <sstream>

#ifndef TARGET_Linux
#include "WorkerDesktop.hpp"
#include "UpdatePureOS.h"
#endif

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
#ifdef TARGET_RT1051
    std::unique_ptr<UpdatePureOS> updateOS;
#endif

  private:
#ifdef TARGET_RT1051
    std::unique_ptr<WorkerDesktop> desktopWorker;
#endif
    static const char *serviceName;
};
