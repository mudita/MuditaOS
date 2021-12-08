// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>

#include <application-settings/windows/WindowNames.hpp>
#include <apps-common/audio/SoundsPlayer.hpp>
#include <bsp/common.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <EventStore.hpp>
#include <application-settings/models/bluetooth/BluetoothSettingsModel.hpp>
#include <application-settings/data/WallpaperOption.hpp>

class AudioStopNotification; // Forward declaration

namespace app
{
    inline constexpr auto name_settings = "ApplicationSettings";

    namespace settingsInterface
    {
        class SimParams
        {
          public:
            virtual ~SimParams()                     = default;
            virtual void setSim(Store::GSM::SIM sim) = 0;
            virtual void updateSim()                 = 0;
            virtual Store::GSM::SIM getSim()         = 0;
            virtual std::string getNumber()          = 0;
        };
        class OperatorsSettings
        {
          public:
            virtual ~OperatorsSettings()                               = default;
            virtual void setOperatorsOn(bool value)                    = 0;
            [[nodiscard]] virtual bool getOperatorsOn() const noexcept = 0;
            virtual void setVoLTEOn(bool value)                        = 0;
            [[nodiscard]] virtual bool getVoLTEOn() const noexcept     = 0;
        };
        class ScreenLightSettings
        {
          public:
            struct Values
            {
                bool lightOn;
                screen_light_control::ScreenLightMode mode;
                screen_light_control::ManualModeParameters parameters;
            };

            virtual ~ScreenLightSettings()                      = default;
            virtual auto getCurrentValues() -> Values           = 0;
            virtual void setBrightness(float brightnessValue)   = 0;
            virtual void setMode(bool isAutoLightSwitchOn)      = 0;
            virtual void setStatus(bool isDisplayLightSwitchOn) = 0;
        };

        class KeypdBacklightSettings
        {
          public:
            virtual ~KeypdBacklightSettings()                                        = default;
            virtual auto getKeypadBacklightState() -> bsp::keypad_backlight::State   = 0;
            virtual void setKeypadBacklightState(bsp::keypad_backlight::State state) = 0;
        };

        class WallpaperSettings
        {
          public:
            virtual ~WallpaperSettings()                                 = default;
            virtual auto getWallpaperOption() -> gui::WallpaperOption    = 0;
            virtual void setWallpaperOption(gui::WallpaperOption option) = 0;
        };

        class DndSettings
        {
          public:
            virtual ~DndSettings() = default;

            virtual auto getNotificationsWhenLocked() const noexcept -> bool = 0;
            virtual void setNotificationsWhenLocked(bool on) noexcept        = 0;
            virtual auto getCallsFromFavourite() const noexcept -> bool      = 0;
            virtual void setCallsFromFavourite(bool on) noexcept             = 0;
        };

        class ConnectionSettings
        {
          public:
            virtual ~ConnectionSettings() = default;

            virtual auto getConnectionFrequency() const noexcept -> uint8_t = 0;
            virtual void setConnectionFrequency(uint8_t val) noexcept       = 0;
        };

        class OfflineSettings
        {
          public:
            virtual auto isFlightMode() const noexcept -> bool     = 0;
            virtual void setFlightMode(bool flightModeOn) noexcept = 0;
        };

        class AutoLockSettings
        {
          public:
            virtual auto getAutoLockTime() -> std::chrono::seconds      = 0;
            virtual void setAutoLockTime(std::chrono::seconds lockTime) = 0;
        };

        class PhoneModeSettings
        {
          public:
            virtual auto getCurrentPhoneMode() const noexcept -> sys::phone_modes::PhoneMode = 0;
        };

    }; // namespace settingsInterface

    class ApplicationSettings : public app::Application,
                                public settingsInterface::SimParams,
                                public settingsInterface::OperatorsSettings,
                                public settingsInterface::ScreenLightSettings,
                                public settingsInterface::KeypdBacklightSettings,
                                public settingsInterface::WallpaperSettings,
                                public settingsInterface::DndSettings,
                                public settingsInterface::OfflineSettings,
                                public settingsInterface::ConnectionSettings,
                                public settingsInterface::AutoLockSettings,
                                public settingsInterface::PhoneModeSettings,
                                public AsyncCallbackReceiver
    {
      public:
        explicit ApplicationSettings(std::string name                    = name_settings,
                                     std::string parent                  = {},
                                     StatusIndicators statusIndicators   = StatusIndicators{},
                                     StartInBackground startInBackground = {false});
        ~ApplicationSettings() override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;
        auto InitHandler() -> sys::ReturnCodes override;

        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        void setSim(Store::GSM::SIM sim) override;
        void updateSim() override;
        Store::GSM::SIM getSim() override;
        std::string getNumber() override;

        void operatorOnChanged(const std::string &value);
        void setOperatorsOn(bool value) override;
        bool getOperatorsOn() const noexcept override;
        void setVoLTEOn(bool value) override;
        bool getVoLTEOn() const noexcept override;
        void volteChanged(const std::string &value);
        void setOsUpdateVersion(const std::string &value);

        ScreenLightSettings::Values getCurrentValues() override;
        void setBrightness(float brightnessValue) override;
        void setMode(bool isAutoLightSwitchOn) override;
        void setStatus(bool isDisplayLightSwitchOn) override;

        auto getKeypadBacklightState() -> bsp::keypad_backlight::State override;
        void setKeypadBacklightState(bsp::keypad_backlight::State keypadLightState) override;

        auto getWallpaperOption() -> gui::WallpaperOption override;
        void setWallpaperOption(gui::WallpaperOption option) override;

        auto getNotificationsWhenLocked() const noexcept -> bool override;
        void setNotificationsWhenLocked(bool on) noexcept override;
        auto getCallsFromFavourite() const noexcept -> bool override;
        void setCallsFromFavourite(bool on) noexcept override;

        auto isFlightMode() const noexcept -> bool override;
        void setFlightMode(bool flightModeOn) noexcept override;

        auto getConnectionFrequency() const noexcept -> uint8_t override;
        void setConnectionFrequency(uint8_t val) noexcept override;

        auto getAutoLockTime() -> std::chrono::seconds override;
        void setAutoLockTime(std::chrono::seconds lockTime) override;

        auto getCurrentPhoneMode() const noexcept -> sys::phone_modes::PhoneMode override;

      private:
        void attachQuotesWindows();
        void switchToAllDevicesViaBtErrorPrompt(std::shared_ptr<sys::DataMessage> msg, const std::string &errorMsg);

        auto handleAudioStop(AudioStopNotification *notification) -> sys::MessagePointer;

        std::shared_ptr<SoundsPlayer> soundsPlayer;
        Store::GSM::SIM selectedSim   = Store::GSM::get()->selected;
        std::string selectedSimNumber = {};

        bool operatorsOn              = false;
        bool voLteStateOn             = false;
        bool notificationsWhenLocked  = true;
        bool callsFromFavorites       = false;
        int connectionFrequency       = 0;
        bool flightModeOn             = true;
        std::shared_ptr<BluetoothSettingsModel> bluetoothSettingsModel = nullptr;
    };

    template <> struct ManifestTraits<ApplicationSettings>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch,
                     manager::actions::PhoneModeChanged,
                     manager::actions::BluetoothModeChanged,
                     manager::actions::AlarmClockStatusChanged}};
        }
    };
} /* namespace app */
