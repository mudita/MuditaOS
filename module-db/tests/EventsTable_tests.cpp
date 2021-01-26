// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Databases/EventsDB.hpp"
#include "Tables/EventsTable.hpp"

#include <purefs/filesystem_paths.hpp>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <purefs/filesystem_paths.hpp>
#include <unistd.h>

using namespace std::chrono_literals;

static auto remove_events(EventsDB &db) -> bool
{
    auto count   = db.events.count();
    auto records = db.events.getLimitOffset(0, count);
    bool ret     = true;
    for (auto rec : records) {
        ret = ret && db.events.removeById(rec.ID);
    }
    return ret;
}
