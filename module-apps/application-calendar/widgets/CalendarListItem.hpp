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
    };

} /* namespace gui */
