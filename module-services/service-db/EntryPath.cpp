// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/EntryPath.hpp"
#include <Split.hpp>
#include <tuple>

namespace settings
{

    void EntryPath::parse(const std::string &dbPath)
    {
        auto parts = utils::split(dbPath, "\\", false);
        if (1 == parts.size()) {
            variable = dbPath;
            scope    = SettingsScope::Global;
        }
        else {
            mode     = parts[0];
            service  = parts[1];
            profile  = parts[2];
            variable = parts[3];
            scope    = SettingsScope::AppLocal;
        }
    }

    bool operator<(const EntryPath &lhs, const EntryPath &rhs) noexcept
    {
        if (lhs.scope != rhs.scope) {
            return lhs.scope < rhs.scope;
        }

        // Scopes are equal - compare other internals.
        if (lhs.scope == SettingsScope::AppLocal) {
            return std::tie(lhs.scope, lhs.mode, lhs.service, lhs.profile, lhs.variable) <
                   std::tie(rhs.scope, rhs.mode, rhs.service, rhs.profile, rhs.variable);
        }
        return lhs.variable < rhs.variable;
    }
} // namespace settings
