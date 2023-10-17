// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsScope.hpp"
#include <string>
#include <type_traits>

namespace settings
{
    struct EntryPath
    {
        std::string mode = "";
        std::string service;
        std::string profile = "";
        std::string variable;
        SettingsScope scope;

        [[nodiscard]] auto to_string(std::string sep = "\\") const -> std::string
        {
            if (SettingsScope::Global == scope) {
                return variable;
            }
            return mode + sep + service + sep + profile + sep + variable;
        }

        void parse(const std::string &dbPath);
    };

    bool operator<(const settings::EntryPath &lhs, const settings::EntryPath &rhs) noexcept;

} // namespace settings
