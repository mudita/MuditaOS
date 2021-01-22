// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "widgets/BarGraph.hpp"

namespace style::settings::brightness
{
    inline constexpr auto max_value = 6;
};

class UTF8;

namespace gui
{
    class Image;
    class SpinBox : public HBox
    {
      public:
        using UpdateCallback = std::function<bool(uint8_t)>;

        SpinBox(Item *parent,
                const std::string &title,
                UpdateCallback updateCallback,
                std::uint8_t maxValue   = style::settings::brightness::max_value,
                std::uint8_t startValue = 0);

      private:
        auto addArrow(Item *parent, const std::string &arrowName, Alignment::Horizontal aligment, bool visible)
            -> Image *;
        auto addBarGraph(Item *parent, uint8_t maxValue, uint8_t startValue) -> HBarGraph *;
        auto addTitle(Item *parent, const std::string &text) -> Label *;

        HBarGraph *bar;
        Label *titleLabel;
        Image *leftArrow;
        Image *rightArrow;
        UpdateCallback updateBarCallback;
    };
} // namespace gui
