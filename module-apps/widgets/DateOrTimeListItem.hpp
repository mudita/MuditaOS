// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItem.hpp>
#include <module-db/Interface/EventsRecord.hpp>

namespace gui
{
    class DateOrTimeListItem : public ListItem
    {
      public:
        std::function<bool()> onContentChangeCallback                           = nullptr;
        std::function<void(std::shared_ptr<EventsRecord> event)> onLoadCallback = nullptr;
        std::function<bool(std::shared_ptr<EventsRecord> event)> onSaveCallback = nullptr;
    };
} /* namespace gui */
