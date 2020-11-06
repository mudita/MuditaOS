// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory> // for allocator, unique_ptr

#include "WorkerDesktop.hpp"
#include "module-services/service-desktop/endpoints/update/UpdateMuditaOS.hpp"
#include "Service/Common.hpp"  // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp" // for MessagePointer, DataMessage (ptr only), ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "Constants.hpp"

class UpdateMuditaOS;
class WorkerDesktop;

namespace sdesktop
{
    inline constexpr auto service_stack         = 8192;
    inline constexpr auto cdc_queue_len         = 10;
    inline constexpr auto cdc_queue_object_size = 10;
}; // namespace sdesktop

class ServiceDesktop : public sys::Service
{
  public:
    ServiceDesktop();
    ~ServiceDesktop() override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

    std::unique_ptr<UpdateMuditaOS> updateOS;
    std::unique_ptr<WorkerDesktop> desktopWorker;
};
