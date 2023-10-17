// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <appmgr/models/WallpaperDisplayModel.hpp>
#include <service-appmgr/model/ApplicationManagerCommon.hpp>

#include <apps-common/locks/handlers/PhoneLockHandler.hpp>
#include <apps-common/locks/handlers/SimLockHandler.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>

namespace app::manager
{
    class SetAutoLockTimeoutRequest;

    class ApplicationManager : public ApplicationManagerCommon
    {
      public:
        ApplicationManager(const ApplicationName &serviceName,
                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                           const ApplicationName &_rootApplicationName);

      private:
        auto InitHandler() -> sys::ReturnCodes override;
        void changePhoneMode(sys::phone_modes::PhoneMode phoneMode, const ApplicationHandle *app);
        void handlePhoneModeChanged(sys::phone_modes::PhoneMode phoneMode);
        void handleBluetoothModeChanged(sys::bluetooth::BluetoothMode mode);
        void handleAlarmClockStatusChanged(bool status);
        void changeBluetoothMode(const ApplicationHandle *app);
        void changeAlarmClockStatus(const ApplicationHandle *app);
        void handleTetheringChanged(sys::phone_modes::Tethering tethering);
        void changeTetheringState(sys::phone_modes::Tethering state, const ApplicationHandle *app);
        void handleSnoozeCountChange(unsigned snoozeCount);
        void handleBatteryStatusChange();
        void processKeypadBacklightState(bsp::keypad_backlight::State keypadLightState);
        void registerMessageHandlers() override;
        void startBackgroundApplications();
        auto handleAutoLockSetRequest(SetAutoLockTimeoutRequest *request) -> std::shared_ptr<sys::ResponseMessage>;
        auto handleDeveloperModeRequest(sys::Message *request) -> sys::MessagePointer override;
        void lockTimeChanged(const std::string &value);
        /// @brief method is called on auto-locking timer tick event (blockTimer)
        /// @detailed It sends AutoLock action to ApplicationDesktop to lock the screen.
        /// @note AutoLock action is sent only if following conditions are met:
        ///  - tethering is off
        ///  - focused application is not preventing AutoLock
        void onPhoneLocked();
        auto startApplication(ApplicationHandle &app) -> bool override;
        auto resolveHomeWindow() -> std::string override;
        auto resolveHomeApplication() -> std::string override;
        auto handleDBResponse(db::QueryResponse *msg) -> bool;
        auto handlePhoneModeChangedAction(ActionEntry &action) -> ActionProcessStatus;
        auto handleAction(ActionEntry &action) -> ActionProcessStatus override;
        void handleStart(StartAllowedMessage *msg) override;
        void runAppsInBackground();
        auto handleDisplayLanguageChange(DisplayLanguageChangeRequest *msg) -> bool override;

        std::shared_ptr<sys::phone_modes::Observer> phoneModeObserver;
        sys::bluetooth::BluetoothMode bluetoothMode = sys::bluetooth::BluetoothMode::Disabled;
        bool alarmClockStatus                       = false;
        locks::PhoneLockHandler phoneLockHandler;
        locks::SimLockHandler simLockHandler;

        notifications::NotificationsConfiguration notificationsConfig;
        notifications::NotificationsHandler notificationsHandler;
        notifications::NotificationProvider notificationProvider;

        //< auto-lock timer to count time from last user's activity.
        // If it reaches time defined in settings database application
        // manager is sending signal to Application Desktop in order to
        // lock screen.
        sys::TimerHandle autoLockTimer;

        wallpaper::WallpaperDisplayModel wallpaperModel;
    };
} // namespace app::manager
