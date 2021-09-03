// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory> // for allocator, unique_ptr

#include "WorkerDesktop.hpp"
#include "Service/Common.hpp"  // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp" // for MessagePointer, DataMessage (ptr only), ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "Timers/TimerHandle.hpp"
#include "Constants.hpp"
#include "USBSecurityModel.hpp"

#include <service-db/DBServiceName.hpp>

namespace settings
{
    class Settings;
}

class UpdateMuditaOS;

namespace sdesktop
{
    inline constexpr auto service_stack             = 8192;
    inline constexpr auto worker_stack              = 8704;
    inline constexpr auto cdc_queue_len             = 32;
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

    enum class Operation
    {
        Backup,
        Restore
    };
    enum class OperationState
    {
        Stopped,
        Running,
        Error
    };

    static const std::string opToString(const OperationState &op)
    {
        switch (op) {
        case OperationState::Stopped:
            return "stopped";
        case OperationState::Running:
            return "running";
        case OperationState::Error:
            return "error";
        default:
            return "unkown";
        }
    }
    struct BackupRestoreStatus
    {
        std::filesystem::path backupTempDir;
        std::filesystem::path location;
        bool lastOperationResult = false;
        std::string task;
        OperationState state = OperationState::Stopped;
        Operation operation  = Operation::Backup;
        json11::Json to_json() const
        {
            return json11::Json::object{{parserFSM::json::task, task},
                                        {parserFSM::json::state, opToString(state)},
                                        {parserFSM::json::location, location.string()}};
        }
    } backupRestoreStatus;

    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    void ProcessCloseReason(sys::CloseReason closeReason) override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

    std::unique_ptr<WorkerDesktop> desktopWorker;

    void prepareBackupData();
    void prepareRestoreData(const std::filesystem::path &restoreLocation);
    const BackupRestoreStatus getBackupRestoreStatus()
    {
        return backupRestoreStatus;
    }
    const sdesktop::USBSecurityModel *getSecurity()
    {
        return usbSecurityModel.get();
    }

    auto requestLogsFlush() -> void;

    auto getSerialNumber() const -> std::string;

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
