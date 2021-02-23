// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <boot/bootconfig.hpp>
#include <boot/bootconstants.hpp>

#include <module-utils/gsl/gsl_util>
#include <limits.h>
#include <purefs/filesystem_paths.hpp>
#include <source/version.hpp>
#include <time/time_conversion.hpp>
#include <ticks.hpp>
#include <cstdio>
#include <log/log.hpp>
#include <crc32/crc32.h>
#include <array>
#include <Utils.hpp>

#include <fstream>

#include <purefs/filesystem_paths.hpp>
#include <sys/mount.h>
#include <sys/statvfs.h>

namespace boot
{
    namespace
    {
        bool updateFileCRC32(const std::filesystem::path &file)
        {
            auto fp = fopen(file.c_str(), "r");
            if (!fp) {
                LOG_WARN("updateFileCRC32 can't open file %s for read", file.c_str());
                return false;
            }
            auto fpCloseAct = gsl::finally([fp] { fclose(fp); });

            unsigned long fileCRC32 = utils::filesystem::computeFileCRC32(fp);
            LOG_INFO("updateFileCRC32 writing new crc32 %08" PRIX32 " for %s",
                     static_cast<std::uint32_t>(fileCRC32),
                     file.c_str());

            if (fileCRC32 == 0) {
                return false;
            }

            std::array<char, boot::consts::crc_char_size + 1> crc32Buf;

            if (int written = sprintf(crc32Buf.data(), "%08" PRIX32, fileCRC32);
                written != boot::consts::crc_char_size) {
                LOG_INFO("updateFileCRC32 can't prepare string for crc32, sprintf returned %d instead of %d",
                         written,
                         boot::consts::crc_char_size);
                return false;
            }

            std::filesystem::path fileCRC32Path = file;
            fileCRC32Path += boot::consts::ext_crc32;

            auto fpCRC32 = fopen(fileCRC32Path.c_str(), "w");
            if (!fpCRC32) {
                LOG_WARN("updateFileCRC32 can't open crc32 file for write");
                return false;
            }
            auto fpCRC32CloseAct = gsl::finally([fpCRC32] { fclose(fpCRC32); });

            if (fwrite(crc32Buf.data(), 1, boot::consts::crc_char_size, fpCRC32) != boot::consts::crc_char_size) {
                LOG_WARN("updateFileCRC32 can't write new crc32");
                return false;
            }

            LOG_INFO("updateFileCRC32 wrote \"%s\" in %s", crc32Buf.data(), fileCRC32Path.c_str());
            return true;
        }

        std::string loadFileAsString(const std::filesystem::path &fileToLoad)
        {
            std::string content;
            std::ifstream in(fileToLoad);
            std::getline(in, content, std::string::traits_type::to_char_type(std::string::traits_type::eof()));
            return content;
        }

        bool verifyCRC(const std::filesystem::path &file, const unsigned long crc32)
        {
            auto fp = fopen(file.c_str(), "r");
            if (!fp) {
                LOG_ERROR("verifyCRC can't open %s", file.c_str());
                return false;
            }
            auto fpCloseAct = gsl::finally([fp] { fclose(fp); });

            unsigned long crc32Read = utils::filesystem::computeFileCRC32(fp);
            LOG_INFO(
                "verifyCRC computed crc32 for %s is %08" PRIX32, file.c_str(), static_cast<std::uint32_t>(crc32Read));

            return crc32Read == crc32;
        }

        bool readAndVerifyCRC(const std::filesystem::path &file)
        {
            std::filesystem::path crcFilePath(file);
            crcFilePath += boot::consts::ext_crc32;

            auto fp = fopen(crcFilePath.c_str(), "r");
            if (!fp) {
                LOG_ERROR("verifyCRC can't open %s", crcFilePath.c_str());
                return false;
            }
            auto fpCloseAct = gsl::finally([fp] { fclose(fp); });

            std::array<char, boot::consts::crc_char_size + 1> crcBuf;
            size_t readSize = std::fread(crcBuf.data(), 1, boot::consts::crc_char_size, fp);
            if (readSize != boot::consts::crc_char_size) {
                LOG_ERROR("verifyCRC fread on %s returned different size then %d [%zu]",
                          crcFilePath.c_str(),
                          boot::consts::crc_char_size,
                          readSize);
                return false;
            }

            crcBuf[boot::consts::crc_char_size] = 0;
            const unsigned long crc32Read       = strtoull(crcBuf.data(), nullptr, boot::consts::crc_radix);

            LOG_INFO("verifyCRC read %s string:\"%s\" hex:%08lX", crcFilePath.c_str(), crcBuf.data(), crc32Read);
            return verifyCRC(file, crc32Read);
        }
    } // namespace

    BootConfig::BootConfig() : m_os_root_path(purefs::dir::getRootDiskPath())
    {}

    json11::Json BootConfig::to_json() const
    {
        return json11::Json::object{
            {boot::json::main,
             json11::Json::object{{boot::json::os_image, m_os_image},
                                  {boot::json::os_type, m_os_type},
                                  {boot::json::os_version, m_os_version},
                                  {boot::json::timestamp, m_timestamp}}},

            {boot::json::git_info,
             json11::Json::object{{boot::json::os_git_tag, std::string(GIT_TAG)},
                                  {boot::json::os_git_revision, std::string(GIT_REV)},
                                  {boot::json::os_git_branch, std::string(GIT_BRANCH)}}},
            {boot::json::bootloader, json11::Json::object{{boot::json::os_version, m_bootloader_version}}}};
    }

    int BootConfig::load()
    {
        return !loadBootConfig(getCurrentBootJSON());
    }

    // Method to compare two version strings
    //   v1 <  v2  -> -1
    //   v1 == v2  ->  0
    int BootConfig::version_compare(const std::string &v1, const std::string &v2)
    {
        size_t i = 0, j = 0;
        while (i < v1.length() || j < v2.length()) {
            int acc1 = 0, acc2 = 0;

            while (i < v1.length() && v1[i] != '.') {
                acc1 = acc1 * 10 + (v1[i] - '0');
                i++;
            }
            while (j < v2.length() && v2[j] != '.') {
                acc2 = acc2 * 10 + (v2[j] - '0');
                j++;
            }

            if (acc1 < acc2)
                return -1;
            if (acc1 > acc2)
                return +1;

            ++i;
            ++j;
        }
        return 0;
    }

    void BootConfig::updateBootJson(const std::filesystem::path &bootJsonPath)
    {
        m_timestamp = utils::time::Timestamp().str("%c");

        struct statvfs stat;
        if (statvfs(purefs::dir::getRootDiskPath().c_str(), &stat))
            LOG_ERROR("%s: Failed to stat vfs", bootJsonPath.c_str());

        auto remount_ro{true};
        auto flags = stat.f_flag & ~MS_RDONLY;
        if (mount(NULL, purefs::dir::getRootDiskPath().c_str(), NULL, flags | MS_REMOUNT, NULL)) {
            LOG_WARN("%s: Failed to remount filesystem R/W - it's fine for a Linux FS", bootJsonPath.c_str());
            remount_ro = false;
        }

        LOG_INFO("Writing new %s..", bootJsonPath.c_str());
        std::ofstream file(bootJsonPath);
        if (file.is_open()) {
            file << to_json().dump() << std::flush;
            if (!file.good())
                LOG_ERROR("%s: Error while writing a file", bootJsonPath.c_str());
            file.close();
            updateFileCRC32(bootJsonPath);
        }
        else {
            LOG_ERROR("%s: Failed to open file", bootJsonPath.c_str());
        }

        if (remount_ro)
            if (mount(NULL, purefs::dir::getRootDiskPath().c_str(), NULL, flags | MS_RDONLY | MS_REMOUNT, NULL))
                LOG_ERROR("%s: Failed to remount filesystem back to RO", bootJsonPath.c_str());
    }

    bool BootConfig::loadBootConfig(const std::filesystem::path &bootJsonPath)
    {
        std::string parseErrors  = "";
        std::string jsonContents = loadFileAsString(bootJsonPath);

        LOG_INFO("parsed %s: \"%s\"", bootJsonPath.c_str(), jsonContents.c_str());

        m_boot_json_parsed = json11::Json::parse(jsonContents, parseErrors);

        if (parseErrors == "") {
            m_os_type            = m_boot_json_parsed[boot::json::main][boot::json::os_type].string_value();
            m_os_image           = m_boot_json_parsed[boot::json::main][boot::json::os_image].string_value();
            m_os_root_path       = purefs::createPath(purefs::dir::getRootDiskPath(), m_os_type);
            m_boot_json          = bootJsonPath;
            m_bootloader_version = m_boot_json_parsed[boot::json::bootloader][boot::json::os_version].string_value();
            m_timestamp          = utils::time::Timestamp().str("%c");
            m_os_version         = std::string(VERSION);

            LOG_INFO("boot_config: %s", to_json().dump().c_str());
            return true;
        }
        else {
            m_os_type      = purefs::dir::getCurrentOSPath();
            m_os_image     = purefs::file::boot_bin;
            m_os_root_path = purefs::createPath(purefs::dir::getRootDiskPath(), m_os_type);
            m_boot_json    = bootJsonPath;
            m_timestamp    = utils::time::Timestamp().str("%c");
            m_os_version   = std::string(VERSION);
            LOG_WARN("%s failed to parse %s: \"%s\"", __FUNCTION__, bootJsonPath.c_str(), parseErrors.c_str());
            return false;
        }
    }

    std::filesystem::path BootConfig::getCurrentBootJSON()
    {
        auto boot_json_path = purefs::dir::getRootDiskPath() / purefs::file::boot_json;
        if (!readAndVerifyCRC(boot_json_path)) {
            LOG_INFO("CRC check failed on %s", boot_json_path.c_str());
            // replace broken .boot.json with a default one
            updateBootJson(boot_json_path);
        }
        return boot_json_path;
    }
} // namespace boot
