// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

// static lifetime read only cache for (hw) states to not poll
// right now it serves data from:
// - battery
// - gsm SIM tray
// it's not meant to serve as polling interface - rather to serve data

#include <hal/cellular/SIM.hpp>

#include <cstddef>
#include <string>

namespace cpp_freertos
{
    // fw decl
    class MutexStandard;
} // namespace cpp_freertos

namespace Store
{
    struct Battery
    {
      private:
        static bool updated;

      public:
        enum class LevelState : std::uint8_t
        {
            Normal,
            Shutdown,
            CriticalCharging,
            CriticalNotCharging
        } levelState = LevelState::Normal;

        enum class State : std::uint8_t
        {
            Discharging,
            Charging,
            ChargingDone,
            PluggedNotCharging,
        } state            = State::Discharging;
        unsigned int level = 0;

        /// @brief Returns const reference to Battery instance, used to read battery state
        /// @return const Battery&
        static const Battery &get();

        /// @brief Returns reference to Battery instance, used to change battery state
        /// @return Battery&
        static Battery &modify();

        static void setUpdated();
        static bool takeUpdated();
    };

    enum class RssiBar : size_t
    {
        zero  = 0,
        one   = 1,
        two   = 2,
        three = 3,
        four  = 4,
        noOfSupprtedBars
    };

    struct SignalStrength
    {
        int rssi        = 0;
        int rssidBm     = 0;
        RssiBar rssiBar = RssiBar::zero;
    };

    enum class Tethering
    {
        Off,
        On
    };

    struct Network
    {
        enum class Status
        {
            NotRegistered,
            RegisteredHomeNetwork,
            Searching,
            RegistrationDenied,
            Unknown,
            RegisteredRoaming
        } status = Status::NotRegistered;

        enum class AccessTechnology
        {
            Gsm   = 0,
            Utran = 2,
            GsmWEgprs,
            UtranWHsdpa,
            UtranWHsupa,
            UtranWHsdpaAndWHsupa,
            EUtran,
            Cdma    = 100,
            Unknown = 255
        } accessTechnology = AccessTechnology::Unknown;

        inline bool operator==(const Network &rhs)
        {
            return this->status == rhs.status && this->accessTechnology == rhs.accessTechnology;
        }
        inline bool operator!=(const Network &rhs)
        {
            return !(*this == rhs);
        }
    };

    struct GSM
    {
      private:
        GSM() = default;
        SignalStrength signalStrength;
        Network network;
        std::string networkOperatorName;
        Tethering tethering;

        static cpp_freertos::MutexStandard mutex;

      public:
        GSM(const GSM &) = delete;
        GSM &operator=(const GSM &) = delete;

        enum class Tray
        {
            OUT,
            IN
        } tray = Tray::IN;
        /// tray - tray actual status which is visible right now on screen
        /// selected - tray selection settings settable sim tray
        enum class SIM
        {
            SIM1 = static_cast<int>(hal::cellular::SimSlot::SIM1),
            SIM2 = static_cast<int>(hal::cellular::SimSlot::SIM2),
            SIM_FAIL,
            SIM_UNKNOWN,
            NONE,
        } sim      = SIM::SIM_UNKNOWN,
          selected = SIM::SIM1;

        bool simCardInserted();

        /// state of modem
        enum class Modem
        {
            OFF,             /// modem is off - it's not working
            ON_INITIALIZING, /// modem is set to on, just started - initialization not done yet
            ON_NEED_SIMFLOW, /// modem is on, initialized, no SIM initialization yet
            ON_INITIALIZED,  /// modem is on, and it's fully initialized
        } modem = Modem::OFF;

        void setSignalStrength(const SignalStrength &signalStrength);
        SignalStrength getSignalStrength() const;

        void setNetwork(const Network &network);
        Network getNetwork() const;

        void setNetworkOperatorName(const std::string &newNetworkOperatorName);
        std::string getNetworkOperatorName() const;

        void setTethering(const Tethering &tethering);
        Tethering getTethering() const;

        static GSM *get();
    };
}; // namespace Store
