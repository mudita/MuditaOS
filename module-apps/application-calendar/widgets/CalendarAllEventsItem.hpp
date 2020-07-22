#pragma once
#include <ListItem.hpp>
#include <Label.hpp>

namespace gui
{
    class CalendarAllEventsItem : public ListItem
    {
        gui::Label *startTime   = nullptr;
        gui::Label *description = nullptr;

      public:
        CalendarAllEventsItem();
        virtual ~CalendarAllEventsItem() = default;

        void setMarkerItem(UTF8 text);
        UTF8 getLabelMarker();

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
