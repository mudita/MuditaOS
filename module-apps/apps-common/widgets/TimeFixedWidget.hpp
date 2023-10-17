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
            explicit LeftBox(std::uint32_t digitsCount);
            LeftBox();
            static constexpr std::uint32_t defaultSize{3};
            HBox *box    = nullptr;
            Label *minus = nullptr;
            DigitsContainer container;
        };

        struct RightBox
        {
            explicit RightBox(std::uint32_t digitsCount);
            RightBox();
            static constexpr std::uint32_t defaultSize{2};
            HBox *box = nullptr;
            DigitsContainer container;
        };

        TimeFixedWidget(Item *parent,
                        const std::uint32_t &x,
                        const std::uint32_t &y,
                        const std::uint32_t &w,
                        const std::uint32_t &h,
                        bool minus = false);

        TimeFixedWidget(Item *parent,
                        const std::uint32_t &x,
                        const std::uint32_t &y,
                        const std::uint32_t &w,
                        const std::uint32_t &h,
                        bool minus,
                        std::uint32_t leftBoxSize,
                        std::uint32_t rightBoxSize);

        void init(std::uint32_t w, std::uint32_t h);

        void setMinutesBox(std::uint32_t first);
        void setSecondsBox(std::uint32_t second);
        void setFontAndDimensions(const UTF8 &fontName) const;

        DimensionsParams getInitialDimensions() const;

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
