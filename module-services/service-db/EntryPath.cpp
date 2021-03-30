// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-db/EntryPath.hpp"
#include <Split.hpp>

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

    namespace
    {
        bool isLessAppLocal(const EntryPath &lhs, const EntryPath &rhs) noexcept
        {
            if (lhs.mode < rhs.mode) {
                return true;
            }
            if (lhs.mode > rhs.mode) {
                return false;
            }
            if (lhs.service < rhs.service) {
                return true;
            }
            if (lhs.service > rhs.service) {
                return false;
            }
            if (lhs.profile < rhs.profile) {
                return true;
            }
            if (lhs.profile > rhs.profile) {
                return false;
            }
            if (lhs.variable < rhs.variable) {
                return true;
            }
            if (lhs.variable > rhs.variable) {
                return false;
            }
            return false;
        }
    } // namespace

    bool operator<(const EntryPath &lhs, const EntryPath &rhs) noexcept
    {
        if (lhs.scope != rhs.scope) {
            return lhs.scope < rhs.scope;
        }

        // Scopes are equal - compare other internals.
        if (lhs.scope == SettingsScope::AppLocal) {
            return isLessAppLocal(lhs, rhs);
        }
        return lhs.variable < rhs.variable;
    }
} // namespace settings
