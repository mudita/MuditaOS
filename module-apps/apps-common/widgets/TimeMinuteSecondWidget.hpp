// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>
#include <BoxLayout.hpp>
#include "AbstractProgressTime.hpp"

namespace gui
{
    class TimeMinuteSecondWidget : public Rect, public AbstractProgressTime
    {
      public:
        enum class DisplayType
        {
            MinutesThenSeconds,
            OnlyMinutes,
            OnlySeconds,
        };

        TimeMinuteSecondWidget(Item *parent,
                               const std::uint32_t &x,
                               const std::uint32_t &y,
                               const std::uint32_t &w,
                               const std::uint32_t &h,
                               DisplayType type = DisplayType::OnlyMinutes);

        void updateTime(std::uint32_t sec) override;
        void buildInterface(std::uint32_t w, std::uint32_t h);
        void setText(std::uint32_t value);

      private:
        DisplayType displayType;
        std::optional<std::uint32_t> totalSeconds;

        static constexpr auto maxDigits{3U};
        VBox *mainBox{nullptr};
        HBox *descriptionBox{nullptr};
        HBox *digitsContainer{nullptr};
        VBox *digitBox[maxDigits]{nullptr};
        Label *digitsText[maxDigits]{nullptr};
        Label *description{nullptr};
    };
} // namespace gui
