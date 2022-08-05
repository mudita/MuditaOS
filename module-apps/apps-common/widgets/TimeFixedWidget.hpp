// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "DigitsContainer.hpp"
#include <Text.hpp>
#include <BoxLayout.hpp>
#include "widgets/DateWidget.hpp"
#include <time/FromTillDate.hpp>

namespace gui
{

    class TimeFixedWidget : public Rect
    {
      public:
        struct LeftBox
        {
            HBox *box    = nullptr;
            Label *minus = nullptr;
            DigitsContainer<3> container{};
        };

        struct RightBox
        {
            HBox *box = nullptr;
            DigitsContainer<2> container{};
        };

        TimeFixedWidget(Item *parent,
                        const std::uint32_t &x,
                        const std::uint32_t &y,
                        const std::uint32_t &w,
                        const std::uint32_t &h,
                        const bool minus = false);

        void setMinutesBox(std::uint32_t first);
        void setSecondsBox(std::uint32_t second);
        void setFontAndDimensions(const UTF8 &fontName) const;

        DimensionsParams getDimensions() const;

      private:
        void attachLabelToBox(Label *&label, HBox *&box) const;
        void setMinus() const;
        void setColon() const;
        void setDimensions(DimensionsParams &&params) const;

        bool minusVisible = false;
        HBox *mainBox     = nullptr;
        Label *colon      = nullptr;

        LeftBox leftBox;
        RightBox rightBox;
    };

} /* namespace gui */
