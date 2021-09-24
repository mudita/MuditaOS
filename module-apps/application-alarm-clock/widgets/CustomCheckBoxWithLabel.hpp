// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItem.hpp>
#include <CheckBoxWithLabel.hpp>

#include <application-calendar/data/CalendarData.hpp>

namespace gui
{
    class CustomCheckBoxWithLabel : public ListItem
    {
      private:
        gui::CheckBoxWithLabel *checkBoxWithLabel = nullptr;
        bool initialState                         = false;

      public:
        CustomCheckBoxWithLabel(const std::string &description,
                                bool initialState,
                                const std::function<void(const UTF8 &text)> &bottomBarTemporaryMode = nullptr,
                                const std::function<void()> &bottomBarRestoreFromTemporaryMode      = nullptr);
        bool isChecked();
    };
} // namespace gui
