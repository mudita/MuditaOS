// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/widgets/DateWidget.hpp>
#include <widgets/DateOrTimeListItem.hpp>

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
