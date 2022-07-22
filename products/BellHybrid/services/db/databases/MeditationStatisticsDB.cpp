// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationStatisticsDB.hpp"

namespace db::meditation_stats
{
    MeditationStatisticsDB::MeditationStatisticsDB(const char *name) : Database(name), table(this)
    {}

} // namespace db::meditation_stats
