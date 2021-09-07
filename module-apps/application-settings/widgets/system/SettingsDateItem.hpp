// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/DateOrTimeListItem.hpp>
#include <widgets/DateWidget.hpp>

namespace gui
{
    class SettingsDateItem : public DateOrTimeListItem
    {
      public:
        SettingsDateItem();

        [[nodiscard]] DateWidget *getDateWidget() const noexcept
        {
            return dateWidget;
        }

      private:
        DateWidget *dateWidget = nullptr;
        VBox *vBox             = nullptr;
    };
} /* namespace gui */
