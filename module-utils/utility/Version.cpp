// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Version.hpp"

#include <module-utils/i18n/JSONLoader.hpp>
#include <log/log.hpp>

namespace utils
{

    std::optional<std::string> getPreviousVersionFromFile(const std::filesystem::path &path)
    {
        const auto &load = utils::JSONLoader(path);
        if (not load.has_value()) {
            LOG_ERROR("Cannot parse %s to json", path.c_str());
            return {};
        }
        const auto &json = load.value();
        return json["prevOsVersion"].string_value();
    }

} // namespace utils
