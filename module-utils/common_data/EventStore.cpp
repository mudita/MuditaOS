// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventStore.hpp"
#include <log/log.hpp>
#include <memory>
#include <mutex.hpp>
#include <vfs.hpp>
#include <module-vfs/include/user/purefs/filesystem_paths.hpp>
#include <Utils.hpp>
#include <source/version.hpp>
#include <module-utils/time/time_conversion.hpp>

namespace Store
{
    // if it grows bigger than these few variables - consider moving it to ram with i.e.
    // delayed construction singletone
    Battery battery;

    const Battery &Battery::get()
    {
        return battery;
    }

    Battery &Battery::modify()
    {
        return battery;
    }

    cpp_freertos::MutexStandard GSM::mutex;

    GSM *GSM::get()
    {
        static auto ptr = new GSM();
        return ptr;
    }

    void GSM::setSignalStrength(const SignalStrength &signalStrength)
    {
        cpp_freertos::LockGuard lock(mutex);
        LOG_INFO("Setting signal strength to rssi = %d dBm (%d) : %d bars",
                 signalStrength.rssidBm,
                 signalStrength.rssi,
                 static_cast<int>(signalStrength.rssiBar));

        this->signalStrength = signalStrength;
    }

    SignalStrength GSM::getSignalStrength() const
    {
        cpp_freertos::LockGuard lock(mutex);

        return signalStrength;
    }

    void GSM::setNetwork(const Network &network)
    {
        cpp_freertos::LockGuard lock(mutex);
        this->network = network;
    }

    Network GSM::getNetwork() const
    {
        cpp_freertos::LockGuard lock(mutex);

        return network;
    }

    bool GSM::simCardInserted()
    {
        return (sim == SIM::SIM1 || sim == SIM::SIM2);
    }

    /**
     * BootConfig holds all information in .boot.json
     * @return
     */

    cpp_freertos::MutexStandard BootConfig::bootConfigMutex;
    BootConfig *BootConfig::get()
    {
        static auto ptr = new BootConfig();
        return ptr;
    }

    void BootConfig::setRootPath(const std::filesystem::path &newRootPath)
    {
        lockMutex(bootConfigMutex);
        os_root_path = newRootPath;
    }

    const std::filesystem::path BootConfig::getOSRootPath()
    {
        lockMutex(bootConfigMutex);
        return (os_root_path);
    }

    const std::string BootConfig::getOSType()
    {
        lockMutex(bootConfigMutex);
        return (os_type);
    }

    const std::string BootConfig::getBootloaderVersion()
    {
        lockMutex(bootConfigMutex);
        return (bootloader_verion);
    }

    bool BootConfig::loadBootConfig(const std::filesystem::path &bootJsonPath)
    {
        std::string parseErrors  = "";
        std::string jsonContents = utils::filesystem::loadFileAsString(fs::path("/sys") / bootJsonPath);

        LOG_INFO("loadBootConfig parsing %s", (fs::path("/sys") / bootJsonPath).c_str());
        LOG_INFO("loadBootConfig \"%s\"", jsonContents.c_str());

        boot_json_parsed = json11::Json::parse(jsonContents, parseErrors);

        lockMutex(bootConfigMutex);
        if (parseErrors == "") {
            os_type           = boot_json_parsed[BootConfigJson::main][BootConfigJson::os_type].string_value();
            os_image          = boot_json_parsed[BootConfigJson::main][BootConfigJson::os_image].string_value();
            os_root_path      = purefs::dir::getRootDiskPath() / os_type;
            boot_json         = bootJsonPath;
            bootloader_verion = boot_json_parsed[BootConfigJson::bootloader][BootConfigJson::os_version].string_value();
            timestamp         = utils::time::Timestamp().str("%c");
            os_version        = std::string(VERSION);

            LOG_INFO("boot_config: %s", to_json().dump().c_str());
            return true;
        }
        else {
            os_type      = PATH_CURRENT;
            os_image     = purefs::file::boot_bin;
            os_root_path = purefs::dir::getRootDiskPath() / os_type;
            boot_json    = bootJsonPath;
            timestamp    = utils::time::Timestamp().str("%c");
            os_version   = std::string(VERSION);

            LOG_WARN("loadBootConfig failed to parse %s: \"%s\"", bootJsonPath.c_str(), parseErrors.c_str());
            return false;
        }
    }

    void BootConfig::updateTimestamp()
    {
        timestamp = utils::time::Timestamp().str("%c");
        LOG_INFO("vfs::updateTimestamp \"%s\"", to_json().dump().c_str());

        if (utils::filesystem::replaceWithString(boot_json, to_json().dump())) {
            utils::filesystem::updateFileCRC32(boot_json);
        }
    }

    json11::Json BootConfig::to_json() const
    {
        return json11::Json::object{
            {BootConfigJson::main,
             json11::Json::object{{BootConfigJson::os_image, os_image},
                                  {BootConfigJson::os_type, os_type},
                                  {BootConfigJson::os_version, os_version},
                                  {BootConfigJson::timestamp, timestamp}}},

            {BootConfigJson::git_info,
             json11::Json::object{{BootConfigJson::os_git_tag, std::string(GIT_TAG)},
                                  {BootConfigJson::os_git_revision, std::string(GIT_REV)},
                                  {BootConfigJson::os_git_branch, std::string(GIT_BRANCH)}}},
            {BootConfigJson::bootloader, json11::Json::object{{BootConfigJson::os_version, bootloader_verion}}}};
    }
}; // namespace Store
