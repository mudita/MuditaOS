// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/BellSideListItem.hpp>
#include <widgets/DateSetSpinner.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    class DateSetListItem : public BellSideListItem
    {
      public:
        DateSetSpinner *dateSetSpinner = nullptr;

        DateSetListItem(const UTF8 &topDesc = "", DateSetSpinner::Type type = DateSetSpinner::Type::year);

        /// called before next SideListItem is activated
        /// @param `this` : item
        std::function<void(Item &)> onNextCallback;
    };

    class YearSetListItem : public DateSetListItem
    {
      public:
        YearSetListItem(const UTF8 &topDesc = "");
    };

    class MonthSetListItem : public DateSetListItem
    {
      public:
        MonthSetListItem(const UTF8 &topDesc = "");
    };

    class DaySetListItem : public DateSetListItem
    {
      public:
        DaySetListItem(const UTF8 &topDesc = "");
    };

} /* namespace gui */
