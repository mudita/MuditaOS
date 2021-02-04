// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <module-gui/gui/widgets/Image.hpp>

namespace gui
{
    class EventDetailDescriptionItem : public CalendarListItem
    {
        gui::VBox *vBox        = nullptr;
        gui::HBox *hBox         = nullptr;
        gui::Label *title      = nullptr;
        gui::Label *eventTime  = nullptr;
        gui::Text *description = nullptr;
        gui::Image *squareImage = nullptr;

      public:
        EventDetailDescriptionItem();
        virtual ~EventDetailDescriptionItem() override = default;

        void descriptionHandler();
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
