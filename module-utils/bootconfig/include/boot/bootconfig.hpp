// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-utils/json/json11.hpp>
#include <string>
#include <filesystem>

namespace boot
{
    class BootConfig
    {
      public:
        BootConfig();
        static int version_compare(const std::string &v1, const std::string &v2);
        [[nodiscard]] json11::Json to_json() const;
        int load();
        int save();
        auto os_image() -> const std::string &
        {
            return m_os_image;
        }
        auto os_type() -> const std::string &
        {
            return m_os_type;
        }
        auto os_version() -> const std::string &
        {
            return m_os_version;
        }
        auto bootloader_version() -> const std::string &
        {
            return m_bootloader_version;
        }
        auto timestamp() -> const std::string &
        {
            return m_timestamp;
        }
        auto os_root_path() -> const std::filesystem::path &
        {
            return m_os_root_path;
        }
        void updateTimestamp();

      private:
        bool loadBootConfig(const std::filesystem::path &bootJsonPath);
        std::filesystem::path getCurrentBootJSON() const;

      private:
        std::string m_os_image{"boot.bin"};
        std::string m_os_type{"current"};
        std::string m_os_version;
        std::string m_bootloader_version;
        std::string m_timestamp;
        json11::Json m_boot_json_parsed;
        std::filesystem::path m_os_root_path;
        std::filesystem::path m_boot_json;
    };
} // namespace boot
