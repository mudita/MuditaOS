// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

#include <bsp/common.hpp>
#include <module-services/service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <common_data/EventStore.hpp>

namespace gui::window::name
{
    inline constexpr auto bluetooth   = "Bluetooth";
    inline constexpr auto all_devices = "AllDevices";
    inline constexpr auto phone_name  = "PhoneName";
    inline constexpr auto add_device  = "AddDevice";

    inline constexpr auto network        = "Network";
    inline constexpr auto apn_settings   = "APNSettings";
    inline constexpr auto apn_options    = "APNOptions";
    inline constexpr auto phone_modes    = "PhoneModes";
    inline constexpr auto apps_and_tools = "AppsAndTools";
    inline constexpr auto security       = "Security";
    inline constexpr auto system         = "System";

    inline constexpr auto display_light  = "DisplayLight";
    inline constexpr auto font_size      = "FontSize";
    inline constexpr auto keypad_light   = "KeypadLight";
    inline constexpr auto input_language = "InputLanguage";
    inline constexpr auto locked_screen  = "LockedScreen";

    inline constexpr auto phone              = "Phone";
    inline constexpr auto call_ringtone      = "CallRingtone";
    inline constexpr auto messages           = "Messages";
    inline constexpr auto message_sound      = "MessageSound";
    inline constexpr auto message_templates  = "MessageTemplates";
    inline constexpr auto calendar           = "Calendar";
    inline constexpr auto notification_sound = "NotificationSound";
    inline constexpr auto alarm_clock        = "AlarmClock";
    inline constexpr auto torch              = "Torch";
    inline constexpr auto nightshift         = "Nightshift";

    inline constexpr auto autolock             = "Autolock";
    inline constexpr auto wallpaper            = "Wallpaper";
    inline constexpr auto quotes               = "Quotes";
    inline constexpr auto new_quote            = "NewQuote";
    inline constexpr auto edit_quote           = "EditQuote";
    inline constexpr auto edit_quotes          = "EditQuotes";
    inline constexpr auto options_quote        = "OptionsQuote";
    inline constexpr auto delete_quote         = "DeleteQuote";
    inline constexpr auto quotes_dialog_yes_no = "DialogYesNo";
    inline constexpr auto quote_categories     = "QuoteCategories";

    inline constexpr auto display_and_keypad = "DisplayAndKeypad";
    inline constexpr auto change_settings    = "ChangeSettings";
    inline constexpr auto all_operators      = "AllOperators";
    inline constexpr auto pin_settings       = "PINSettings";
    inline constexpr auto import_contacts    = "ImportContacts";
    inline constexpr auto dialog_settings    = "DialogSettings";
    inline constexpr auto change_passcode    = "ChangePasscode";

    inline constexpr auto languages             = "Languages";
    inline constexpr auto date_and_time         = "DateAndTime";
    inline constexpr auto factory_reset         = "FactoryReset";
    inline constexpr auto about_your_pure       = "AboutYourPure";
    inline constexpr auto technical_information = "TechnicalInformation";
    inline constexpr auto certification         = "Certification";
    inline constexpr auto sar                   = "Sar";

    inline constexpr auto change_date_and_time = "ChangeDateAndTime";
    inline constexpr auto change_time_zone     = "ChangeTimeZone";

    inline constexpr auto new_apn                 = "NewApn";
    inline constexpr auto bluetooth_check_passkey = "BluetoothCheckPasskey";

    inline constexpr auto do_not_disturb       = "DoNotDisturb";
    inline constexpr auto offline              = "Offline";
    inline constexpr auto connection_frequency = "ConnectionFrequency";

} // namespace gui::window::name

namespace app
{
    inline constexpr auto name_settings_new = "ApplicationSettingsNew";

    namespace settingsInterface
    {
        class SimParams
        {
          public:
            virtual ~SimParams()                     = default;
            virtual void setSim(Store::GSM::SIM sim) = 0;
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
                screen_light_control::Parameters parameters;
            };

            virtual ~ScreenLightSettings()                      = default;
            virtual auto getCurrentValues() -> Values           = 0;
            virtual void setBrightness(float brigtnessValue)    = 0;
            virtual void setMode(bool isAutoLightSwitchOn)      = 0;
            virtual void setStatus(bool isDisplayLightSwitchOn) = 0;
            virtual void setBrightnessFunction()                = 0;
        };

        class KeypdBacklightSettings
        {
          public:
            virtual ~KeypdBacklightSettings()                   = default;
            virtual auto isKeypadBacklightOn() -> bool          = 0;
            virtual void setKeypadBacklightState(bool newState) = 0;
        };

        class SecuritySettings
        {
          public:
            virtual ~SecuritySettings() = default;

            virtual auto isUSBSecured() const -> bool  = 0;
            virtual void setUSBSecurity(bool security) = 0;
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

        class ConnectedSettings
        {
          public:
            virtual auto isMessageSoundEnabled() const noexcept -> bool               = 0;
            virtual void setMessageSoundEnabled(bool messageSoundOn) noexcept         = 0;
            virtual auto isMessageVibrationEnabled() const noexcept -> bool           = 0;
            virtual void setMessageVibrationEnabled(bool messageVibrationOn) noexcept = 0;
        };

    }; // namespace settingsInterface

    class ApplicationSettingsNew : public app::Application,
                                   public settingsInterface::SimParams,
                                   public settingsInterface::OperatorsSettings,
                                   public settingsInterface::ScreenLightSettings,
                                   public settingsInterface::KeypdBacklightSettings,
                                   public settingsInterface::SecuritySettings,
                                   public settingsInterface::DndSettings,
                                   public settingsInterface::OfflineSettings,
                                   public settingsInterface::ConnectedSettings,
                                   public settingsInterface::ConnectionSettings,
                                   public AsyncCallbackReceiver
    {
      public:
        explicit ApplicationSettingsNew(std::string name                    = name_settings_new,
                                        std::string parent                  = {},
                                        sys::phone_modes::PhoneMode mode    = sys::phone_modes::PhoneMode::Connected,
                                        StartInBackground startInBackground = {false});
        ~ApplicationSettingsNew() override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::MessagePointer override;
        auto InitHandler() -> sys::ReturnCodes override;

        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        void setSim(Store::GSM::SIM sim) override;
        Store::GSM::SIM getSim() override;
        std::string getNumber() override;

        void operatorOnChanged(const std::string &value);
        void setOperatorsOn(bool value) override;
        bool getOperatorsOn() const noexcept override;
        void setVoLTEOn(bool value) override;
        bool getVoLTEOn() const noexcept override;
        void volteChanged(const std::string &value);
        void setOsUpdateVersion(const std::string &value);
        [[nodiscard]] auto getLockPassHash() const noexcept -> unsigned int
        {
            return lockPassHash;
        }
        void setLockPassHash(unsigned int value);

        ScreenLightSettings::Values getCurrentValues() override;
        void setBrightness(float brigtnessValue) override;
        void setMode(bool isAutoLightSwitchOn) override;
        void setStatus(bool isDisplayLightSwitchOn) override;
        void setBrightnessFunction() override;

        auto isKeypadBacklightOn() -> bool override;
        void setKeypadBacklightState(bool newState) override;

        auto isUSBSecured() const -> bool override;
        void setUSBSecurity(bool security) override;

        void setLockScreenPasscodeOn(bool passcodeOn);
        auto isLockScreenPasscodeOn() const -> bool;

        auto getNotificationsWhenLocked() const noexcept -> bool override;
        void setNotificationsWhenLocked(bool on) noexcept override;
        auto getCallsFromFavourite() const noexcept -> bool override;
        void setCallsFromFavourite(bool on) noexcept override;

        auto isFlightMode() const noexcept -> bool override;
        void setFlightMode(bool flightModeOn) noexcept override;

        auto isMessageSoundEnabled() const noexcept -> bool override;
        void setMessageSoundEnabled(bool messageSoundOn) noexcept override;
        auto isMessageVibrationEnabled() const noexcept -> bool override;
        void setMessageVibrationEnabled(bool messageVibrationOn) noexcept override;

        auto getConnectionFrequency() const noexcept -> uint8_t override;
        void setConnectionFrequency(uint8_t val) noexcept override;

      private:
        void attachQuotesWindows();

        Store::GSM::SIM selectedSim   = Store::GSM::get()->selected;
        std::string selectedSimNumber = {};
        bsp::Board board              = bsp::Board::none;
        bool operatorsOn              = false;
        bool voLteStateOn             = false;
        bool usbSecured               = true;
        bool lockScreenPasscodeOn     = true;
        unsigned int lockPassHash     = 0;
        bool notificationsWhenLocked  = true;
        bool callsFromFavorites       = false;
        int connectionFrequency       = 0;
        bool flightModeOn             = true;
        bool connectedMessageSoundOn     = true;
        bool connectedMessageVibrationOn = true;
    };

    template <> struct ManifestTraits<ApplicationSettingsNew>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::PhoneModeChanged}};
        }
    };
} /* namespace app */
