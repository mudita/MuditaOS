// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Tables/Record.hpp"
#include "module-db/Common/Common.hpp"
#include <application-calendar/data/dateCommon.hpp>
#include <utf8/UTF8.hpp>
#include <stdint.h>

struct EventInfo
{
    UTF8 name{""};
    TimePoint startDate{TIME_POINT_INVALID};
    TimePoint endDate{TIME_POINT_INVALID};
    uint32_t duration{0};
    bool isAllDay{false};
};

struct EventRecord : public Record, public EventInfo
{
    uint32_t id{0};
    std::string rrule{""};
};

struct SingleEventRecord : public Record, public EventInfo
{
    std::shared_ptr<EventRecord> parent;
};
