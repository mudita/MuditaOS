// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EventRecord.hpp"
#include "module-db/Common/Common.hpp"
#include <application-calendar/data/dateCommon.hpp>
#include <utf8/UTF8.hpp>
#include <stdint.h>

struct AlarmEventRecord : public EventRecord
{
    std::string musicTone{""};
    bool enabled{false};
};
