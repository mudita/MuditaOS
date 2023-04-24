// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>

namespace settings
{
    namespace factory
    {
        static constexpr auto entry_key         = "factory_data";
        static constexpr auto serial_number_key = "serial";
        static constexpr auto case_colour_key   = "case_colour";
    }

    class FactorySettings
    {
      public:
        virtual ~FactorySettings()                                                       = default;
        [[nodiscard]] virtual auto getMfgEntries() const -> std::unique_ptr<QueryResult> = 0;
        virtual auto initDb(Database *database) const -> void                            = 0;
    };
} // namespace settings
