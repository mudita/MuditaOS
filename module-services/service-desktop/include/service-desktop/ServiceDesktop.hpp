// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <bsp/usb/usb.hpp>
#include "Constants.hpp"
#include "ServiceDesktopName.hpp"
#include "ServiceDesktopDependencies.hpp"
#include "USBSecurityModel.hpp"
#include "DeveloperModeMessage.hpp"
#include "DesktopMessages.hpp"
#include <locks/data/PhoneLockMessages.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/ResponseVisibleDevices.hpp>
#include <service-desktop/Sync.hpp>
#include <service-desktop/OutboxNotifications.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-desktop/WorkerDesktop.hpp>

namespace settings
{
    class Settings;
}

namespace sdesktop
{
    using namespace std::chrono_literals;

    inline constexpr auto serviceStackSize = 1024 * 8;
    inline constexpr auto workerStackSize  = 1024 * 9;

    inline constexpr auto cdcReceiveQueueName          = "cdcReceiveQueueBuffer";
    inline constexpr auto cdcReceiveQueueItemSize      = sizeof(std::string *);
    inline constexpr auto cdcReceiveQueueLength        = 1024;

    inline constexpr auto cdcSendQueueName             = "cdcSendQueueBuffer";
    inline constexpr auto cdcSendQueueItemSize         = sizeof(std::string *);
    inline constexpr auto cdcSendQueueLength           = 1024;

    inline constexpr auto irqQueueName     = "irqQueueBuffer";
    inline constexpr auto irqQueueItemSize = sizeof(bsp::USBDeviceStatus);
    inline constexpr auto irqQueueLength   = 1;

    inline constexpr auto signallingQueueName     = "signallingQueueBuffer";
    inline constexpr auto signallingQueueItemSize = sizeof(WorkerDesktop::Signal);
    inline constexpr auto signallingQueueLength   = 1;

    inline constexpr auto connectionActiveTimerName    = "connectionActiveTimer";
    inline constexpr auto connectionActiveTimerDelayMs = 20s;

    inline constexpr auto deviceUniqueIdName   = "sd_device_unique_id";
    inline constexpr auto deviceUniqueIdLength = 32;

    inline constexpr auto pathFactoryDataSerial     = "factory_data/serial";
    inline constexpr auto pathFactoryDataCaseColour = "factory_data/case_colour";
}; // namespace sdesktop

namespace sdesktop::bluetooth
{
    class BluetoothMessagesHandler;
}

class ServiceDesktop : public sys::Service
{
  public:
    explicit ServiceDesktop(const std::filesystem::path &mtpRootPath);
    ~ServiceDesktop() override;

    std::unique_ptr<WorkerDesktop> desktopWorker;
    Sync::OperationStatus syncStatus;

    auto InitHandler() -> sys::ReturnCodes override;
    auto DeinitHandler() -> sys::ReturnCodes override;
    auto SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes override;
    auto DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) -> sys::MessagePointer override;

    auto prepareSyncData() -> void;
    auto getSyncStatus() const -> Sync::OperationStatus
    {
        return syncStatus;
    }
    auto getSecurity() const -> const sdesktop::USBSecurityModel *
    {
        return usbSecurityModel.get();
    }

    auto requestLogsFlush() -> void;

    auto getSerialNumber() const -> std::string;
    auto getCaseColour() const -> std::string;
    auto getDeviceToken() -> std::string;

    auto getNotificationEntries() const -> std::vector<Outbox::NotificationEntry>;
    auto removeNotificationEntries(const std::vector<std::uint32_t> &uidsOfNotificationsToBeRemoved) -> void;
    auto getMtpPath() const noexcept -> std::filesystem::path;
    auto getOnboardingState() const -> sdesktop::endpoints::OnboardingState;

  private:
    std::unique_ptr<sdesktop::USBSecurityModel> usbSecurityModel;
    std::unique_ptr<settings::Settings> settings;
    std::unique_ptr<sdesktop::bluetooth::BluetoothMessagesHandler> btMsgHandler;
    OutboxNotifications outboxNotifications;
    sys::TimerHandle connectionActiveTimer;
    static constexpr unsigned int defaultLogFlushTimeoutMs   = 1000U;
    bool initialized                                         = false;
    bool isPlugEventUnhandled                                = false;
    bool isUsbConfigured                                     = false;
    std::filesystem::path mtpRootPath;

    auto generateDeviceUniqueId() -> void;
    auto getDeviceUniqueId() const -> std::string;
    auto setDeviceUniqueId(const std::string &token) -> void;

    auto usbWorkerInit() -> sys::ReturnCodes;
    auto usbWorkerDeinit() -> sys::ReturnCodes;

    auto restartConnectionActiveTimer() -> void;

    auto checkChargingCondition() -> void;

    auto cleanFileSystemEndpointUndeliveredTransfers() -> void;

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
    [[nodiscard]] auto handle(sdesktop::SyncMessage *msg) -> std::shared_ptr<sys::Message>;
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
            manifest.dependencies = sys::dependencies::getDependenciesFor<ServiceDesktop>();
            return manifest;
        }
    };
} // namespace sys
