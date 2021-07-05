// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalendarListItem.hpp"
#include <apps-common/widgets/TimeWidget.hpp>

namespace gui
{
    class VBox;
    class CalendarTimeItem : public CalendarListItem
    {
      public:
        CalendarTimeItem(const std::string &description,
                         TimeWidget::Type type,
                         const std::function<void(const UTF8 &text)> &bottomBarTemporaryMode,
                         const std::function<void()> &bottomBarRestoreFromTemporaryMode,
                         TimePoint dateFrom,
                         TimePoint dateTill);

        void setConnectionToSecondItem(CalendarTimeItem *item);
        void setConnectionToDateItem(CalendarDateItem *item);
        auto getFromTillDate() const -> std::shared_ptr<utils::time::FromTillDate>;

      private:
        TimeWidget *timeWidget = nullptr;
        VBox *vBox             = nullptr;
    };
} /* namespace gui */
