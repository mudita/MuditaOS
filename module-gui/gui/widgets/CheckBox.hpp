// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include "Image.hpp"
#include "NavBar.hpp"

namespace gui
{
    class CheckBox : public HBox
    {
        bool checkState                                              = false;
        Image *image                                                 = nullptr;
        std::function<void(const UTF8 &text)> navBarTemporaryMode    = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode         = nullptr;
        nav_bar::Side navBarSide                                     = nav_bar::Side::Left;

        void applyCallbacks();

      public:
        CheckBox(Item *parent,
                 const uint32_t &x,
                 const uint32_t &y,
                 const uint32_t &w,
                 const uint32_t &h,
                 const std::function<void(const UTF8 &text)> &navBarTemporaryMode = nullptr,
                 const std::function<void()> &navBarRestoreFromTemporaryMode      = nullptr,
                 nav_bar::Side navBarSide                                         = nav_bar::Side::Left);

        void setCheck(bool state);
        bool isChecked();
    };

} /* namespace gui */
