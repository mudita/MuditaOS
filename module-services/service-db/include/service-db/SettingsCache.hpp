// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsMessages.hpp"
#include <map>

namespace settings
{
    class SettingsCache
    {
      public:
        const std::string &getValue(const EntryPath &path) const;
        void setValue(const EntryPath &path, const std::string &value);
        static SettingsCache *getInstance();
        virtual ~SettingsCache() = default;
    };
} // namespace settings
