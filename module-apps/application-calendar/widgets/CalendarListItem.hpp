// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/EventsRecord.hpp>
#include <ListItem.hpp>

namespace gui
{
    using CalendarListItem = ListItemWithCallbacks<EventsRecord>;
} /* namespace gui */
