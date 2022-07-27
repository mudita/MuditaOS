// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Text.hpp>
#include <BoxLayout.hpp>
#include "widgets/DateWidget.hpp"
#include <time/FromTillDate.hpp>

namespace gui
{
    struct DimensionsParams
    {
        std::uint32_t mainBoxHeight;
        std::uint32_t mainBoxWidth;
        std::uint32_t digitMaxWidth;
        std::uint32_t colonWidth;
        std::uint32_t minusWidth;
        std::uint32_t leftBoxWidth;
        std::uint32_t rightBoxWidth;
    };

    class TimeFixedWidget : public Rect
    {
      public:
        template <size_t N> using DigitsContainer = std::array<Label *, N>;

        struct LeftBox
        {
            HBox *box    = nullptr;
            Label *minus = nullptr;
            DigitsContainer<3> digits{nullptr};
        };

        struct RightBox
        {
            HBox *box = nullptr;
            DigitsContainer<2> digits{nullptr};
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

        const LeftBox &getLeftBox();
        const RightBox &getRightBox();

      private:
        void attachLabelToBox(Label *&label, HBox *&box) const;
        void setMinus() const;
        void setColon() const;
        void setDimensions(DimensionsParams &&params) const;
        DimensionsParams getDimensions() const;
        template <size_t N> void setDigits(std::string &&text, const DigitsContainer<N> &digits) const;

        bool minusVisible = false;
        HBox *mainBox     = nullptr;
        Label *colon      = nullptr;

        LeftBox leftBox;
        RightBox rightBox;
    };
} /* namespace gui */
