// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CalendarListItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class EventDateItem : public CalendarListItem
    {
        gui::VBox *vBox        = nullptr;
        gui::HBox *labelsHBox  = nullptr;
        gui::HBox *dateHBox    = nullptr;
        gui::Label *dayLabel   = nullptr;
        gui::Label *monthLabel = nullptr;
        gui::Label *yearLabel  = nullptr;
        gui::Text *dayInput    = nullptr;
        gui::Text *monthInput  = nullptr;
        gui::Text *yearInput   = nullptr;

        void buildInterface();
        void applyItemSpecificProperties(gui::Text *item);
        void applyCallbacks();
        calendar::YearMonthDay validateDate();

      public:
        EventDateItem();

        const calendar::YearMonthDay getChosenDate();
        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
