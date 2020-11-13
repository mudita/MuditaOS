// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventStore.hpp"
#include <log/log.hpp>
#include <memory>
#include <mutex.hpp>
#include <vfs.hpp>
#include <vfs_paths.hpp>
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

    BootConfig bootConfig;
    const BootConfig &BootConfig::get()
    {
        return bootConfig;
    }

    BootConfig &BootConfig::modify()
    {
        return bootConfig;
    }

    std::string BootConfig::loadFileAsString(const std::filesystem::path &fileToLoad)
    {
        auto lamb = [](FILE *stream) { fclose(stream); };
        std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
        std::unique_ptr<FILE, decltype(lamb)> fp(fopen(fileToLoad.c_str(), "r"), lamb);
        std::string contents;
        size_t readSize;

        if (fp.get() != nullptr) {
            while (feof(fp.get())) {
                readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fp.get());
                contents.append(static_cast<const char *>(readBuf.get()), readSize);
            }
        }

        return contents;
    }

    bool BootConfig::loadBootConfig(const std::filesystem::path &bootJsonPath)
    {
        std::string parseErrors  = "";
        std::string jsonContents = loadFileAsString(bootJsonPath);

        LOG_INFO("vfs::getOSRootFromJSON parsing %s", bootJsonPath.c_str());
        LOG_INFO("vfs::getOSRootFromJSON \"%s\"", jsonContents.c_str());

        bootConfig.boot_json_parsed = json11::Json::parse(jsonContents, parseErrors);

        if (parseErrors == "") {
            bootConfig.os_type =
                bootConfig.boot_json_parsed[BootConfigJson::main][BootConfigJson::os_type].string_value();
            bootConfig.os_image =
                bootConfig.boot_json_parsed[BootConfigJson::main][BootConfigJson::os_image].string_value();
            bootConfig.os_root_path = fs::path(purefs::dir::eMMC_disk) / bootConfig.os_type;
            bootConfig.boot_json    = bootJsonPath;
            bootConfig.bootloader_verion =
                bootConfig.boot_json_parsed[BootConfigJson::bootloader][BootConfigJson::os_version].string_value();
            bootConfig.timestamp  = utils::time::Timestamp().str("%c");
            bootConfig.os_version = std::string(VERSION);

            LOG_INFO("boot_config: %s", bootConfig.to_json().dump().c_str());
            return true;
        }
        else {
            bootConfig.os_type      = PATH_CURRENT;
            bootConfig.os_image     = purefs::file::boot_bin;
            bootConfig.os_root_path = fs::path(purefs::dir::eMMC_disk) / bootConfig.os_type;
            bootConfig.boot_json    = bootJsonPath;
            bootConfig.timestamp    = utils::time::Timestamp().str("%c");
            bootConfig.os_version   = std::string(VERSION);

            LOG_WARN("vfs::getOSRootFromJSON failed to parse %s: \"%s\"", bootJsonPath.c_str(), parseErrors.c_str());
            return false;
        }
    }

    const fs::path BootConfig::getCurrentBootJSON()
    {
        return purefs::file::boot_json;
    }

    void BootConfig::updateTimestamp()
    {
        bootConfig.timestamp = utils::time::Timestamp().str("%c");
        LOG_INFO("vfs::updateTimestamp \"%s\"", bootConfig.to_json().dump().c_str());

        if (utils::filesystem::replaceWithString(bootConfig.boot_json, bootConfig.to_json().dump())) {
            utils::filesystem::updateFileCRC32(bootConfig.boot_json);
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
