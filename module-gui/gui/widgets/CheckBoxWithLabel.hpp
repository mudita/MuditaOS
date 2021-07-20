// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CheckBox.hpp"
#include "Label.hpp"
#include "BottomBar.hpp"

namespace gui
{
    class CheckBoxWithLabel : public gui::HBox
    {
      private:
        gui::CheckBox *check = nullptr;
        gui::Label *label    = nullptr;

      public:
        CheckBoxWithLabel(Item *parent,
                          uint32_t x,
                          uint32_t y,
                          uint32_t w,
                          uint32_t h,
                          const UTF8 &description,
                          const std::function<void(const UTF8 &text)> &bottomBarTemporaryMode = nullptr,
                          const std::function<void()> &bottomBarRestoreFromTemporaryMode      = nullptr,
                          BottomBar::Side bottomBarSide                                       = BottomBar::Side::LEFT);

        void setChecked(bool state);
        auto isChecked() const -> bool;
    };
} // namespace gui
