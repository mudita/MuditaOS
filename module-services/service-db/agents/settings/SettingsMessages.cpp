// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/SettingsMessages.hpp>
#include <module-utils/Utils.hpp>

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
} // namespace settings
