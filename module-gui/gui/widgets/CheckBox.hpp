// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include "Image.hpp"
#include "BottomBar.hpp"

namespace gui
{
    class CheckBox : public HBox
    {
        bool checkState                                              = false;
        Image *image                                                 = nullptr;
        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;
        BottomBar::Side bottomBarSide                                = BottomBar::Side::LEFT;

        void applyCallbacks();

      public:
        CheckBox(Item *parent,
                 const uint32_t &x,
                 const uint32_t &y,
                 const uint32_t &w,
                 const uint32_t &h,
                 const std::function<void(const UTF8 &text)> &bottomBarTemporaryMode = nullptr,
                 const std::function<void()> &bottomBarRestoreFromTemporaryMode      = nullptr,
                 BottomBar::Side bottomBarSide                                       = BottomBar::Side::LEFT);

        void setCheck(bool state);
        bool isChecked();
    };

} /* namespace gui */
