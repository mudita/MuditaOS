// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarListItem.hpp"

namespace gui
{
    class DateWidget;
    class VBox;
    class CalendarDateItem : public CalendarListItem
    {
      public:
        CalendarDateItem();

        [[nodiscard]] DateWidget *getDateWidget() const noexcept
        {
            return dateWidget;
        }

      private:
        DateWidget *dateWidget = nullptr;
        VBox *vBox             = nullptr;
    };
} /* namespace gui */
