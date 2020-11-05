// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WorkerDesktop.hpp"
#include "module-services/service-desktop/endpoints/update/UpdateMuditaOS.hpp"
#include "Service/Common.hpp"  // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp" // for MessagePointer, DataMessage (ptr only), ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "Constants.hpp"
#include "WorkerDesktop.hpp"

#include <endpoints/update/UpdateMuditaOS.hpp>

#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include <memory>

namespace sdesktop
{
    const inline int service_stack         = 8192;
    const inline int cdc_queue_len         = 32;
    const inline int cdc_queue_object_size = 1024;
    const inline int file_transfer_timeout = 5000;
    inline constexpr auto RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    inline constexpr auto SEND_QUEUE_BUFFER_NAME    = "sendQueueBuffer";
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

    // upload and download from the service perspective (download to phone, upload to computer)
    sys::ReturnCodes startDownload(const fs::path &destinationFile, const uint32_t fileSize);
    sys::ReturnCodes startUpload(const fs::path &sourceFile, const uint32_t fileSize);

    std::unique_ptr<UpdateMuditaOS> updateOS;
    std::unique_ptr<WorkerDesktop> desktopWorker;
};
