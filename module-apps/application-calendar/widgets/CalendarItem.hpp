#pragma once

#include "Label.hpp"
#include "ListItem.hpp"
#include "application-calendar/models/CalendarModel.hpp"

namespace gui

{
    class CalendarItem : public ListItem
    {
        gui::Label *title       = nullptr;
        gui::Label *description = nullptr;
        gui::Image *clock       = nullptr;

      public:
        CalendarItem();
        virtual ~CalendarItem() = default;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
