// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>
#include <log/log.hpp>
#include <service-db/Settings.hpp>

#include <string>
#include <optional>

namespace app::common
{
    template <typename T> [[nodiscard]] std::optional<T> to_value(const std::string &str)
    {
        /// Should be replaced with https://en.cppreference.com/w/cpp/utility/from_chars but for the time being GCC10
        /// doesn't support it fully

        if constexpr (std::is_integral_v<T>) {
            try {
                return std::stol(str);
            }
            catch (const std::exception &e) {
            }
        }
        else if constexpr (std::is_floating_point_v<T>) {
            try {
                return std::stof(str);
            }
            catch (const std::exception &e) {
            }
        }

        LOG_ERROR("Could not convert to specified value type");
        return {};
    }

    template <typename T> std::optional<T> getSettingsValue(settings::Settings &settings, const std::string &str)
    {
        const auto retStr = settings.getValue(str, settings::SettingsScope::Global);
        if (retStr.empty()) {
            LOG_ERROR("%s does not exist", str.c_str());
            return {};
        }
        else {
            if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, UTF8>) {
                return retStr;
            }
            else {
                return to_value<T>(retStr);
            }
        }
    }

} // namespace app::common
