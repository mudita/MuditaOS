// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <boot/bootconfig.hpp>

#include <module-utils/gsl/gsl_util>
#include <purefs/filesystem_paths.hpp>
#include <source/version.hpp>
#include <time/time_conversion.hpp>
#include <ticks.hpp>
#include <stdio.h>
#include <log/log.hpp>
#include <crc32/crc32.h>
#include <array>
#include <Utils.hpp>

namespace boot
{
    namespace
    {
        bool writeToFile(const fs::path &fileToModify, const std::string &stringToWrite)
        {
            auto fp = fopen(fileToModify.c_str(), "w");
            if (!fp) {
                return false;
            }

            size_t dataWritten = fwrite(stringToWrite.c_str(), stringToWrite.length(), 1, fp);

            fclose(fp);

            return dataWritten == 1;
        }

        bool updateFileCRC32(const fs::path &file)
        {
            auto fp = fopen(file.c_str(), "r");
            if (!fp) {
                LOG_WARN("updateFileCRC32 can't open file %s for write", file.c_str());
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

            std::array<char, purefs::buffer::crc_char_size> crc32Buf;

            if (int written = sprintf(crc32Buf.data(), "%08" PRIX32, fileCRC32);
                written != purefs::buffer::crc_char_size - 1) {
                LOG_INFO("updateFileCRC32 can't prepare string for crc32, sprintf returned %d instead of %d",
                         written,
                         purefs::buffer::crc_char_size - 1);
                return false;
            }

            fs::path fileCRC32Path = file;
            fileCRC32Path += purefs::extension::crc32;

            auto fpCRC32 = fopen(fileCRC32Path.c_str(), "w");
            if (!fpCRC32) {
                LOG_WARN("updateFileCRC32 can't open crc32 file for write");
                return false;
            }
            auto fpCRC32CloseAct = gsl::finally([fpCRC32] { fclose(fpCRC32); });

            if (fwrite(crc32Buf.data(), 1, purefs::buffer::crc_char_size, fpCRC32) != purefs::buffer::crc_char_size) {
                LOG_WARN("updateFileCRC32 can't write new crc32");
                return false;
            }

            LOG_INFO("updateFileCRC32 wrote \"%s\" in %s", crc32Buf.data(), fileCRC32Path.c_str());
            return true;
        }

        std::string loadFileAsString(const fs::path &fileToLoad)
        {
            auto lamb = [](::FILE *stream) { ::fclose(stream); };
            std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
            std::unique_ptr<::FILE, decltype(lamb)> fp(fopen(fileToLoad.c_str(), "r"), lamb);
            std::string contents;
            size_t readSize;

            if (fp.get() != nullptr) {
                while (!feof(fp.get())) {
                    readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fp.get());
                    contents.append(static_cast<const char *>(readBuf.get()), readSize);
                }
            }

            return contents;
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
            fs::path crcFilePath(file);
            crcFilePath += purefs::extension::crc32;

            auto fp = fopen(crcFilePath.c_str(), "r");
            if (!fp) {
                LOG_ERROR("verifyCRC can't open %s", crcFilePath.c_str());
                return false;
            }
            auto fpCloseAct = gsl::finally([fp] { fclose(fp); });

            std::array<char, purefs::buffer::crc_char_size> crcBuf;

            if (size_t readSize = ::fread(crcBuf.data(), 1, purefs::buffer::crc_char_size, fp);
                readSize != purefs::buffer::crc_char_size) {
                LOG_ERROR("verifyCRC fread on %s returned different size then %d [%zu]",
                          crcFilePath.c_str(),
                          purefs::buffer::crc_char_size,
                          readSize);
                return false;
            }

            const unsigned long crc32Read = strtoull(crcBuf.data(), nullptr, purefs::buffer::crc_radix);

            LOG_INFO("verifyCRC read %s string:\"%s\" hex:%08lX", crcFilePath.c_str(), crcBuf.data(), crc32Read);
            return verifyCRC(file, crc32Read);
        }
    } // namespace

    BootConfig::BootConfig() : m_os_root_path(purefs::dir::getRootDiskPath())
    {}
    json11::Json BootConfig::to_json() const
    {
        return json11::Json::object{
            {purefs::json::main,
             json11::Json::object{{purefs::json::os_image, m_os_image},
                                  {purefs::json::os_type, m_os_type},
                                  {purefs::json::os_version, m_os_version},
                                  {purefs::json::timestamp, m_timestamp}}},

            {purefs::json::git_info,
             json11::Json::object{{purefs::json::os_git_tag, std::string(GIT_TAG)},
                                  {purefs::json::os_git_revision, std::string(GIT_REV)},
                                  {purefs::json::os_git_branch, std::string(GIT_BRANCH)}}},
            {purefs::json::bootloader, json11::Json::object{{purefs::json::os_version, m_bootloader_version}}}};
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

    void BootConfig::updateTimestamp()
    {
        m_timestamp = utils::time::Timestamp().str("%c");
        LOG_INFO("vfs::updateTimestamp \"%s\"", to_json().dump().c_str());

        if (writeToFile(m_boot_json, to_json().dump())) {
            updateFileCRC32(m_boot_json);
        }
    }
    bool BootConfig::loadBootConfig(const std::filesystem::path &bootJsonPath)
    {
        std::string parseErrors  = "";
        std::string jsonContents = loadFileAsString(bootJsonPath);

        LOG_INFO("vfs::getOSRootFromJSON parsing %s", bootJsonPath.c_str());
        LOG_INFO("vfs::getOSRootFromJSON \"%s\"", jsonContents.c_str());

        m_boot_json_parsed = json11::Json::parse(jsonContents, parseErrors);

        if (parseErrors == "") {
            m_os_type      = m_boot_json_parsed[purefs::json::main][purefs::json::os_type].string_value();
            m_os_image     = m_boot_json_parsed[purefs::json::main][purefs::json::os_image].string_value();
            m_os_root_path = purefs::createPath(purefs::dir::getRootDiskPath(), m_os_type);
            m_boot_json    = bootJsonPath;
            m_bootloader_version =
                m_boot_json_parsed[purefs::json::bootloader][purefs::json::os_version].string_value();
            m_timestamp  = utils::time::Timestamp().str("%c");
            m_os_version = std::string(VERSION);

            LOG_INFO("boot_config: %s", to_json().dump().c_str());
            return true;
        }
        else {
            m_os_type      = PATH_CURRENT;
            m_os_image     = purefs::file::boot_bin;
            m_os_root_path = purefs::createPath(purefs::dir::getRootDiskPath(), m_os_type);
            m_boot_json    = bootJsonPath;
            m_timestamp    = utils::time::Timestamp().str("%c");
            m_os_version   = std::string(VERSION);
            LOG_WARN("vfs::getOSRootFromJSON failed to parse %s: \"%s\"", bootJsonPath.c_str(), parseErrors.c_str());
            return false;
        }
    }
    std::filesystem::path BootConfig::getCurrentBootJSON() const
    {
        if (readAndVerifyCRC(purefs::file::boot_json)) {
            return purefs::createPath(purefs::dir::getRootDiskPath(), purefs::file::boot_json);
        }
        LOG_INFO("vfs::getCurrentBootJSON crc check failed on %s", purefs::file::boot_json);
        // replace broken .boot.json with a default one
        writeToFile(purefs::dir::getRootDiskPath() / purefs::file::boot_json, to_json().dump());
        return purefs::createPath(purefs::dir::getRootDiskPath(), purefs::file::boot_json);
    }
} // namespace boot
