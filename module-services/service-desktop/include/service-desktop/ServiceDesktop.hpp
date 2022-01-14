// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory> // for allocator, unique_ptr

#include "system/Common.hpp"   // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp" // for MessagePointer, DataMessage (ptr only), ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "Timers/TimerHandle.hpp"
#include "Constants.hpp"
#include "USBSecurityModel.hpp"
#include <service-desktop/BackupRestore.hpp>
#include <service-db/DBServiceName.hpp>

#include <bsp/usb/usb.hpp>

#include <filesystem>

namespace settings
{
    class Settings;
}

class WorkerDesktop;
class UpdateMuditaOS;

namespace sdesktop
{
    inline constexpr auto service_stack             = 8192;
    inline constexpr auto worker_stack              = 8704;
    inline constexpr auto cdc_queue_len             = 1024;
    inline constexpr auto cdc_queue_object_size     = 1024;
    inline constexpr auto irq_queue_object_size     = sizeof(bsp::USBDeviceStatus);
    inline constexpr auto file_transfer_timeout     = 5000;
    inline constexpr auto RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    inline constexpr auto SEND_QUEUE_BUFFER_NAME    = "sendQueueBuffer";
    inline constexpr auto IRQ_QUEUE_BUFFER_NAME     = "irqQueueBuffer";

}; // namespace sdesktop

namespace sdesktop::bluetooth
{
    class BluetoothMessagesHandler;
}

class ServiceDesktop : public sys::Service
{
  public:
    ServiceDesktop();
    ~ServiceDesktop() override;

    BackupRestore::OperationStatus backupRestoreStatus;

    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

    std::unique_ptr<WorkerDesktop> desktopWorker;

    std::string prepareBackupFilename();
    void prepareBackupData();
    void prepareRestoreData(const std::filesystem::path &restoreLocation);
    const BackupRestore::OperationStatus getBackupRestoreStatus()
    {
        return backupRestoreStatus;
    }
    const sdesktop::USBSecurityModel *getSecurity()
    {
        return usbSecurityModel.get();
    }

    auto requestLogsFlush() -> void;

    auto getSerialNumber() const -> std::string;
    auto getCaseColour() const -> std::string;

  private:
    std::unique_ptr<sdesktop::USBSecurityModel> usbSecurityModel;
    std::unique_ptr<settings::Settings> settings;
    std::unique_ptr<sdesktop::bluetooth::BluetoothMessagesHandler> btMsgHandler;

    static constexpr unsigned int DefaultLogFlushTimeoutInMs = 1000U;
    bool initialized                                         = false;
};

namespace sys
{
    template <> struct ManifestTraits<ServiceDesktop>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::service_desktop;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
