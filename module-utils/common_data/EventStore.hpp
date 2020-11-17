// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

// static lifetime read only cache for (hw) states to not poll
// right now it serves data from:
// - battery
// - gsm SIM tray
// it's not meant to serve as polling interface - rather to serve data

#include <cstddef>
#include <filesystem>
#include <module-utils/json/json11.hpp>

#ifndef TARGET_Linux
#define lockMutex(x) cpp_freertos::LockGuard lock(x);
#else
#define lockMutex(x)
#endif

namespace cpp_freertos
{
    // fw decl
    class MutexStandard;
} // namespace cpp_freertos

namespace BootConfigJson
{
    inline constexpr auto main            = "main";
    inline constexpr auto os_type         = "ostype";
    inline constexpr auto os_image        = "imagename";
    inline constexpr auto os_version      = "version";
    inline constexpr auto version_major   = "major";
    inline constexpr auto version_inor    = "minor";
    inline constexpr auto version_patch   = "patch";
    inline constexpr auto version_string  = "string";
    inline constexpr auto timestamp       = "timestamp";
    inline constexpr auto misc            = "misc";
    inline constexpr auto builddate       = "builddate";
    inline constexpr auto git_info        = "git";
    inline constexpr auto os_git_tag      = "git_tag";
    inline constexpr auto os_git_revision = "git_commit";
    inline constexpr auto os_git_branch   = "git_branch";
    inline constexpr auto bootloader      = "bootloader";
}; // namespace BootConfigJson

namespace Store
{
    struct Battery
    {
        enum class State
        {
            Discharging,
            Charging,
        } state            = State::Discharging;
        unsigned int level = 0;

        static const Battery &get();

        static Battery &modify();
    };

    enum class RssiBar : size_t
    {
        zero  = 0,
        one   = 1,
        two   = 2,
        three = 3,
        four  = 4,
        five  = 5,
        noOfSupprtedBars
    };

    struct SignalStrength
    {
        int rssi        = 0;
        int rssidBm     = 0;
        RssiBar rssiBar = RssiBar::zero;
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
            Gsm   = 0x00,
            Utran = 0x02,
            GsmWEgprs,
            UtranWHsdpa,
            UtranWHsupa,
            UtranWHsdpaAndWHsupa,
            EUtran,
            Unknown = 0xFF
        } accessTechnology = AccessTechnology::Unknown;
    };

    struct GSM
    {
      private:
        GSM() = default;

        SignalStrength signalStrength;
        Network network;

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
            SIM1,
            SIM2,
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

        void setNetwork(const Network &signalStrength);

        Network getNetwork() const;

        static GSM *get();
    };

    class BootConfig
    {
      public:
        BootConfig(const BootConfig &) = delete;
        BootConfig &operator=(const BootConfig &) = delete;
        static BootConfig *get();

        void setRootPath(const std::filesystem::path &newRootPath);
        const std::filesystem::path getOSRootPath();
        const std::string getOSType();
        const std::string getBootloaderVersion();

        bool loadBootConfig(const std::filesystem::path &bootJsonPath);
        void updateTimestamp();
        [[nodiscard]] json11::Json to_json() const;
        static int version_compare(const std::string &v1, const std::string &v2);

      private:
        BootConfig() = default;
        std::string os_image;
        std::string os_type;
        std::string os_version;
        std::string bootloader_verion;
        std::string timestamp;
        json11::Json boot_json_parsed;
        std::filesystem::path os_root_path;
        std::filesystem::path boot_json;
        static cpp_freertos::MutexStandard bootConfigMutex;
    };

}; // namespace Store
