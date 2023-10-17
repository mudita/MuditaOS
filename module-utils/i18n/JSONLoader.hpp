// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <optional>
#include <filesystem>
#include <fstream>
#include <log/log.hpp>
#include <json11.hpp>

namespace utils
{
    inline std::optional<json11::Json> JSONLoader(const std::filesystem::path &path)
    {
        auto file = std::ifstream{path};
        if (file.bad()) {
            return {};
        }
        std::stringstream buf{};
        buf << file.rdbuf();

        std::string err;
        json11::Json js = json11::Json::parse(buf.str(), err);

        if (err.length() != 0) {
            LOG_ERROR("Failed to parse input data, err: %s", err.c_str());
            return {};
        }
        else {
            return js;
        }
    }

} // namespace utils
