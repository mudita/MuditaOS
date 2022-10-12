// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

#include <PhoneModes/Common.hpp>
#include <service-bluetooth/Constants.hpp>
#include "status-bar/AlarmClock.hpp"

#include <vector>
#include <map>

namespace gui
{
    class Label;
    class Image;

    namespace status_bar
    {
        class SignalStrengthBase;
        class PhoneMode;
        class BatteryBase;
        class NetworkAccessTechnology;
        class BT;
        class SIM;
        class Time;
        class Lock;
        class Tethering;
    } // namespace status_bar
} // namespace gui

class UTF8;

class StatusBarVisitor;

namespace gui::status_bar
{

    enum class Indicator
    {
        Signal,                  /// signal strenght
        Time,                    /// digital clock
        Lock,                    /// is phone locked
        Battery,                 /// battery status
        Bluetooth,               /// bluetooth status
        SimCard,                 /// sim card info
        NetworkAccessTechnology, /// NAT (eg 3G, 4G, LTE)
        PhoneMode,               /// phone mode
        AlarmClock,              /// alarm clock active
        Tethering,               /// tethering status
    };

    using Indicators          = std::vector<Indicator>;
    using IndicatorStatuses   = std::map<Indicator, bool>;
    using IndicatorsModifiers = std::map<Indicator, std::shared_ptr<StatusBarVisitor>>;

    /// Carries the status bar configuration.
    class Configuration
    {
      public:
        /// Enable specified indicator
        /// @param indicator indicator type to be enabled
        void enable(Indicator indicator);

        /// Enable number of specified indicators
        /// @param indicators vector of indicators to enable
        void enable(const Indicators &indicators);

        /// Disable specified indicator
        /// @param indicator indicator type to disable
        void disable(Indicator indicator);

        /// Set the state of specified indicator
        /// @param indicator indicator type to set state
        /// @param enabled desired status to be set (true=enabled, false=disabled)
        void setIndicator(Indicator indicator, bool enabled);

        /// Set phone mode (connected/dnd/offline)
        /// @param phoneMode desired phone mode configuration
        void setPhoneMode(sys::phone_modes::PhoneMode phoneMode);

        /// Set bluetooth mode
        /// @param bluetoothMode desired bluetooth mode configuration
        void setBluetoothMode(sys::bluetooth::BluetoothMode bluetoothMode);

        /// Set alarm clock status
        /// @param alarmClockStatus desired alarm clock status
        void setAlarmClockStatus(bool alarmClockStatus);

        /// Set tethering state
        /// @param tetheringState desired tethering state
        void setTetheringState(sys::phone_modes::Tethering tetheringState);

        /// Set a configuration modifier to the specified indicator
        /// @param indicator indicator type
        /// @param config desired indicator's configuration
        void setIndicatorModifier(Indicator indicator, std::shared_ptr<StatusBarVisitor> config);

        /// Get the phone mode configuration
        /// @return phone mode
        [[nodiscard]] auto getPhoneMode() const noexcept -> sys::phone_modes::PhoneMode;

        /// Get the bluetooth mode configuration
        /// @return bluetooth mode
        [[nodiscard]] auto getBluetoothMode() const noexcept -> sys::bluetooth::BluetoothMode;

        /// Get the Alarm Clock status configuration
        /// @return alarm clock status
        [[nodiscard]] auto getAlarmClockStatus() const noexcept -> bool;

        /// Get the tethering state configuration
        /// @return tethering state
        [[nodiscard]] auto getTetheringState() const noexcept -> sys::phone_modes::Tethering;

        /// Check if the specified indicator is enabled
        /// @param indicator indicator to be checked
        /// @return indicator status
        [[nodiscard]] auto isEnabled(Indicator indicator) const -> bool;

        /// Return the indicator statuses
        /// @return indicator statuses
        [[nodiscard]] auto getIndicatorsConfiguration() const noexcept -> const IndicatorStatuses &;

        /// Return the indicator modifiers
        /// @return indicator modifiers
        [[nodiscard]] auto getIndicatorsModifiers() const noexcept -> const IndicatorsModifiers &;

      private:
        /// Current indicator statuses
        IndicatorStatuses indicatorStatuses = {
            {Indicator::Signal, false},
            {Indicator::PhoneMode, false},
            {Indicator::Time, false},
            {Indicator::Lock, false},
            {Indicator::Battery, false},
            {Indicator::Bluetooth, false},
            {Indicator::SimCard, false},
            {Indicator::NetworkAccessTechnology, false},
            {Indicator::AlarmClock, false},
            {Indicator::Tethering, false},
        };

        /// Phone mode
        sys::phone_modes::PhoneMode mPhoneMode = sys::phone_modes::PhoneMode::Uninitialized;

        /// Bluetooth mode
        sys::bluetooth::BluetoothMode mBluetoothMode = sys::bluetooth::BluetoothMode::Disabled;

        /// Alarm Clock status
        bool mAlarmClockStatus = false;

        /// Tethering state
        sys::phone_modes::Tethering mTetheringState = sys::phone_modes::Tethering::Off;

        /// Indicator modifiers:
        IndicatorsModifiers indicatorsModifiers;
    };

    /// Status bar widget class.
    /// This is horizontal box with three sections
    /// * left showing signal strenght, NAT info, and/or phone mode
    /// * center showing lock info or digital clock
    /// * right showing sim card and battery status
    class StatusBar : public HBox
    {
      public:
        /// Constructor
        /// @param parent parent item pointer
        /// @param x widget x position
        /// @param y widget y position
        /// @param w widget width
        /// @param h widget height
        StatusBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        /// Set the configuration basing on the specialized gui::top_bar::Configuration class
        /// @param configuration desired configuration
        void configure(Configuration &&config);

        /// Returns the current configuration
        /// @return configuration stored in Configuration class
        [[nodiscard]] auto getConfiguration() const noexcept -> const Configuration &;

        /// Update bluetooth status widget state depending on the current configuration
        bool updateBluetooth(sys::bluetooth::BluetoothMode mode);

        /// Update alarm clock status widget state depending on the current configuration
        bool updateAlarmClock(bool status);

        /// Update sim card status widget state depending on the current configuration
        bool updateSim();

        /// Update clock widget state depending on the current configuration
        bool updateTime();

        /// Update battery status widget state depending on the current configuration
        bool updateBattery();

        /// Update signal widget state depending on the current configuration
        bool updateSignalStrength();

        /// Update phone mode widget state depending on the current configuration
        bool updatePhoneMode();

        /// Update NAT widget state depending on the current configuration
        bool updateNetworkAccessTechnology();

        /// Update tethering widget state depending on the current configuration
        bool updateTetheringState(const sys::phone_modes::Tethering state);

        /// Accepts GuiVisitor to update the status bar
        void accept(GuiVisitor &visitor) override;

      protected:
        /// Set up and add all the widgets to the status bar
        void prepareWidget();

        /// Show/hide bluetooth status widget
        /// @param enabled true to show false to hide the widget
        void showBluetooth(bool enabled);

        /// Show/hide alarm clock status widget
        /// @param enabled true to show false to hide the widget
        void showAlarmClock(bool enabled);

        /// Show/hide sim card status widget
        /// @param enabled true to show false to hide the widget
        void showSim(bool enabled);

        /// Show/hide clock widget
        /// @param enabled true to show false to hide the widget
        bool showTime(bool enabled);

        /// Show/hide lock status widget
        /// @param enabled true to show false to hide the widget
        void showLock(bool enabled);

        /// Show/hide battery status widget
        /// @param enabled true to show false to hide the widget
        /// @return true if screen refresh is required, false if not required
        bool showBattery(bool enabled);

        /// Show/hide signal strenght widget
        /// @param enabled true to show false to hide the widget
        void showSignalStrength(bool enabled);

        /// Show/hide phone mode widget
        /// @param enabled true to show false to hide the widget
        void showPhoneMode(bool enabled);

        /// Show/hide NAT widget
        /// @param enabled true to show false to hide the widget
        void showNetworkAccessTechnology(bool enabled);

        /// Show/hide tethering state widget
        /// @param enabled true to show false to hide the widget
        void showTethering(bool enabled);

        /// Show/hide phone mode or tethering widget
        void showPhoneModeOrTethering(bool phoneModeEnabled, bool tetheringEnabled);

        /// Sets the status of the specified indicator on the Status bar
        /// @param indicator indicator id
        /// @param enabled enable or disable the specified indicator
        void setIndicatorStatus(Indicator indicator, bool enabled);

        /// Applies a modifier to specified indicator
        /// @param indicator indicator id
        /// @param modifier pointer to modifier
        void setIndicatorModifier(Indicator indicator, StatusBarVisitor &modifier);

        /// Pointer to widget showing digital clock
        Time *time = nullptr;

        /// Pointer to widget showing NAT (eg 3G, 4G, LTE)
        NetworkAccessTechnology *networkAccessTechnology = nullptr;

        /// Pointer to widget showing current signal strenght
        SignalStrengthBase *signal = nullptr;

        /// Pointer to widget with current phone mode
        PhoneMode *phoneMode = nullptr;

        /// Pointer to widget showing lock status
        Lock *lock = nullptr;

        /// Pointer to widget with bluetooth status
        BT *bluetooth = nullptr;

        /// Pointer to widget with alarm clock status
        AlarmClock *alarmClock = nullptr;

        /// Pointer to widget with sim card status
        SIM *sim = nullptr;

        /// Pointer to widget with battery status
        BatteryBase *battery = nullptr;

        /// Pointer to widget with tethering state
        Tethering *tethering = nullptr;

        /// Pointer to the left horizontal box
        HBox *leftBox = nullptr;

        /// Pointer to the central horizontal box
        HBox *centralBox = nullptr;

        /// Pointer to the right horizontal box
        HBox *rightBox = nullptr;

        /// Current configuration of the Statusbar
        Configuration configuration;
    };

} // namespace gui::status_bar
