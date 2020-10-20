// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "BoxLayout.hpp"
#include "Image.hpp"
#include <utf8/UTF8.hpp>

namespace gui
{
    class CheckBox : public HBox
    {
        Image *image                                                 = nullptr;
        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;
        bool textOnLeft                                              = true;

        void applyCallbacks();

      public:
        CheckBox(Item *parent,
                 const uint32_t &x,
                 const uint32_t &y,
                 const uint32_t &w,
                 const uint32_t &h,
                 std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                 std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr,
                 bool textOnLeft                                              = true);
        virtual ~CheckBox() override = default;

        void setImageVisible(bool state);
        bool isChecked();
    };

} /* namespace gui */
