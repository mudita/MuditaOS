#pragma once

#include "Label.hpp"
#include "ListItem.hpp"
#include "../models/CalendarModel.hpp"

namespace gui
{
    class CalendarItem : public ListItem
    {

        CalendarModel *model = nullptr;
        // pointer to the ... record
        /// TODO: Connect to database

        gui::Label *title       = nullptr;
        gui::Label *description = nullptr;
        gui::Image *clock       = nullptr;

      public:
        CalendarItem(CalendarModel *model);
        virtual ~CalendarItem();

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        bool onActivated(void *data) override;
    };

} /* namespace gui */