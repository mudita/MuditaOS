// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "MeditationStatisticsDB.hpp"

namespace db::meditation_stats
{
    MeditationStatisticsDB::MeditationStatisticsDB(const char *name) : Database(name), table(this)
    {}

} // namespace db::meditation_stats
