// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/DateOrTimeListItem.hpp>
#include <widgets/TimeWidget.hpp>

namespace gui
{
    class SettingsDateItem;
    class SettingsTimeItem : public DateOrTimeListItem
    {
      public:
        SettingsTimeItem(const std::string &description,
                         TimeWidget::Type type,
                         const std::function<void(const UTF8 &text)> &navBarTemporaryMode,
                         const std::function<void()> &navBarRestoreFromTemporaryMode);

        void setConnectionToSecondItem(SettingsTimeItem *item);
        void setConnectionToDateItem(SettingsDateItem *item);

      private:
        TimeWidget *timeWidget = nullptr;
        VBox *vBox             = nullptr;
    };
} /* namespace gui */
