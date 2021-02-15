// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarListItem.hpp"
#include "Application.hpp"
#include <widgets/DateAndTimeBox.hpp>
#include <CheckBox.hpp>

namespace gui
{
    class CalendarDateAndTimeBox : public CalendarListItem
    {
        app::Application *app          = nullptr;
        DateAndTimeBox *dateAndTimeBox = nullptr;
        gui::VBox *vBox                = nullptr;
        Item *savedFocusItem           = nullptr;

      public:
        CalendarDateAndTimeBox(app::Application *application);

        [[nodiscard]] DateWidget *getDateWidget() const noexcept
        {
            return dateAndTimeBox->getDateWidget();
        }

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };
} /* namespace gui */
