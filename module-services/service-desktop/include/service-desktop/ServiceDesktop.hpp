// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/usb/usb.hpp>
#include "Constants.hpp"
#include "USBSecurityModel.hpp"
#include "DeveloperModeMessage.hpp"
#include "DesktopMessages.hpp"
#include <locks/data/PhoneLockMessages.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include <service-db/DBServiceName.hpp>
#include <service-desktop/BackupRestore.hpp>
#include <service-desktop/OutboxNotifications.hpp>
#include <service-evtmgr/BatteryMessages.hpp>

namespace settings
{
    class Settings;
}

class WorkerDesktop;

namespace sdesktop
{
    inline constexpr auto service_stack             = 8192;
    inline constexpr auto worker_stack              = 8704;
    inline constexpr auto cdcReceiveQueueLength        = 1024;
    inline constexpr auto cdcSendQueueLength           = 1024;
    inline constexpr auto signallingQueueLength        = 4;
    inline constexpr auto irqQueueLength               = 4;
    inline constexpr auto irqQueueSize                 = sizeof(bsp::USBDeviceStatus);
    constexpr auto connectionActiveTimerName        = "connectionActiveTimer";
    constexpr auto connectionActiveTimerDelayMs     = std::chrono::milliseconds{1000 * 20};
    inline constexpr auto RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    inline constexpr auto SEND_QUEUE_BUFFER_NAME    = "sendQueueBuffer";
    inline constexpr auto IRQ_QUEUE_BUFFER_NAME     = "irqQueueBuffer";
    inline constexpr auto SIGNALLING_QUEUE_BUFFER_NAME = "signallingQueueBuffer";
    inline constexpr auto DeviceUniqueIdLength      = 32;
    inline constexpr auto DeviceUniqueIdName        = "sd_device_unique_id";
    constexpr auto pathFactoryDataSerial            = "factory_data/serial";
    constexpr auto pathFactoryDataCaseColour        = "factory_data/case_colour";

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

    std::unique_ptr<WorkerDesktop> desktopWorker;
    BackupRestore::OperationStatus backupRestoreStatus;

    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(sys::ServicePowerMode mode) override;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

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
    auto getDeviceToken() -> std::string;

    auto getNotificationEntries() const -> std::vector<Outbox::NotificationEntry>;
    void removeNotificationEntries(const std::vector<uint32_t> &);

  private:
    std::unique_ptr<sdesktop::USBSecurityModel> usbSecurityModel;
    std::unique_ptr<settings::Settings> settings;
    std::unique_ptr<sdesktop::bluetooth::BluetoothMessagesHandler> btMsgHandler;
    OutboxNotifications outboxNotifications;
    sys::TimerHandle connectionActiveTimer;
    static constexpr unsigned int DefaultLogFlushTimeoutInMs = 1000U;
    bool initialized                                         = false;
    bool isPlugEventUnhandled                                = false;
    bool isUsbConfigured                                     = false;

    void generateDeviceUniqueId();
    auto getDeviceUniqueId() const -> std::string;
    void setDeviceUniqueId(const std::string &token);

    auto usbWorkerInit() -> sys::ReturnCodes;
    auto usbWorkerDeinit() -> sys::ReturnCodes;

    void restartConnectionActiveTimer();

    void checkChargingCondition();

    template <typename T>
    auto connectHandler() -> bool
    {
        return connect(typeid(T), [&](sys::Message *msg) { return handle(static_cast<T *>(msg)); });
    }

    [[nodiscard]] auto handle(db::NotificationMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(locks::UnlockedPhone *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(locks::LockedPhone *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(locks::NextPhoneUnlockAttemptLockTime *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::ResponseStatus *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::ResponseBondedDevices *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(message::bluetooth::ResponseVisibleDevices *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sdesktop::developerMode::DeveloperModeRequest *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sdesktop::BackupMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sdesktop::RestoreMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sdesktop::FactoryMessage *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sdesktop::usb::USBConfigured *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sdesktop::usb::USBDisconnected *msg) -> std::shared_ptr<sys::Message>;
    [[nodiscard]] auto handle(sevm::USBPlugEvent *msg) -> std::shared_ptr<sys::Message>;
};

namespace sys
{
    template <>
    struct ManifestTraits<ServiceDesktop>
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
