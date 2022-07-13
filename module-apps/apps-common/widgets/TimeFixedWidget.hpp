// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Text.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>
#include "widgets/DateWidget.hpp"
#include <time/FromTillDate.hpp>

namespace gui
{
    class TimeFixedWidget : public Rect
    {
      public:
        TimeFixedWidget(Item *parent,
                        const uint32_t &x,
                        const uint32_t &y,
                        const uint32_t &w,
                        const uint32_t &h,
                        const bool minus = false);

        void setFirst(uint32_t first);
        void setSecond(uint32_t second);
        void setMinus(bool minus);
        void setFont(const UTF8 &fontName);

      private:
        bool minusVisible        = false;
        HBox *hBox               = nullptr;
        HBox *firstHBox          = nullptr;
        HBox *secondHBox         = nullptr;
        Label *colonText         = nullptr;
        Label *firstPrimoText    = nullptr;
        Label *firstSecundoText  = nullptr;
        Label *secondPrimoText   = nullptr;
        Label *secondSecundoText = nullptr;
        Label *minusText         = nullptr;
    };
} /* namespace gui */
