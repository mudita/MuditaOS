#pragma once
#include <ListItem.hpp>

namespace gui
{
    class CalendarListItem : public ListItem
    {
      public:
        std::function<void()> onSaveCallback = nullptr;
        std::function<void()> onLoadCallback = nullptr;
    };

} /* namespace gui */
