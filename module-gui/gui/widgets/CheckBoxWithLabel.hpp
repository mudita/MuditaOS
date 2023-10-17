// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CheckBox.hpp"
#include "Label.hpp"
#include "NavBar.hpp"

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
                          const std::function<void(const UTF8 &text)> &navBarTemporaryMode = nullptr,
                          const std::function<void()> &navBarRestoreFromTemporaryMode      = nullptr,
                          nav_bar::Side navBarSide                                         = nav_bar::Side::Left);

        void setChecked(bool state);
        auto isChecked() const -> bool;
    };
} // namespace gui
