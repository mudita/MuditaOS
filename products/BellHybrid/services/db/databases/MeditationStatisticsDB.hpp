// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationStatisticsTable.hpp"
#include <module-db/Database/Database.hpp>

namespace db::meditation_stats
{
    class MeditationStatisticsDB : public Database
    {
      public:
        explicit MeditationStatisticsDB(const char *name);

        MeditationStatsTable table;
    };

} // namespace db::meditation_stats
