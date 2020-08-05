#pragma once
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>

namespace gui
{
    class EventDetailDescriptionItem : public CalendarListItem
    {
        gui::VBox *vBox        = nullptr;
        gui::Label *title      = nullptr;
        gui::Label *eventTime  = nullptr;
        gui::Text *description = nullptr;

      public:
        EventDetailDescriptionItem();
        virtual ~EventDetailDescriptionItem() override = default;

        void descriptionHandler();
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
