#pragma once

#include "Label.hpp"
#include "ListItem.hpp"
#include "../models/CalendarModel.hpp"

namespace gui
{
    class CalendarItem : public ListItem
    {

        CalendarModel *model = nullptr;

        gui::Label *title       = nullptr;
        gui::Label *description = nullptr;
        gui::Image *clock       = nullptr;

      public:
        CalendarItem(CalendarModel *model);

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        bool onActivated(void *data) override;
    };

} /* namespace gui */
