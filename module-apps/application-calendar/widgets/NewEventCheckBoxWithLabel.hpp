// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CheckBoxWithLabelItem.hpp"
#include <widgets/DateWidget.hpp>

namespace gui
{
    class NewEventCheckBoxWithLabel : public CheckBoxWithLabelItem
    {
        using OnCheckCallback     = std::function<void(bool)>;
        OnCheckCallback onCheck   = nullptr;
        app::Application *app    = nullptr;
        gui::DateWidget *dateItem = nullptr;
        void applyCallbacks() override;

      public:
        NewEventCheckBoxWithLabel(app::Application *application,
                                  const std::string &description,
                                  OnCheckCallback onCheck);

        void setConnectionToDateItem(gui::DateWidget *item);
    };

    namespace allDayEvents
    {
        bool isAllDayEvent(TimePoint start, TimePoint end);
    }

} /* namespace gui */
