// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-db/Interface/EventsRecord.hpp>
#include <ListItem.hpp>

namespace gui
{
    class CalendarListItem : public ListItem
    {
      public:
        std::function<void(std::shared_ptr<EventsRecord> event)> onSaveCallback = nullptr;
        std::function<void(std::shared_ptr<EventsRecord> event)> onLoadCallback = nullptr;
        std::function<bool()> onContentChangeCallback                           = nullptr;
    };

} /* namespace gui */
