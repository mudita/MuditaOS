#pragma once

#include "application-calendar/models/DayEventsModel.hpp"
#include <Label.hpp>
#include <ListItem.hpp>

namespace gui

{
    class DayEventsItem : public ListItem
    {
        gui::Label *title       = nullptr;
        gui::Label *description = nullptr;
        gui::Image *clock       = nullptr;

      public:
        DayEventsItem();
        virtual ~DayEventsItem() override = default;

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
