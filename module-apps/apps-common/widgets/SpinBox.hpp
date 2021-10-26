// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "widgets/BarGraph.hpp"

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
                std::uint8_t maxValue,
                std::uint8_t startValue,
                std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr,
                std::function<void()> navBarRestoreFromTemporaryMode      = nullptr);

      private:
        auto addArrow(Item *parent, const std::string &arrowName, Alignment::Horizontal aligment, bool visible)
            -> Image *;
        auto addBarGraph(Item *parent, uint8_t maxValue, uint8_t startValue) -> HBarGraph *;
        auto addTitle(Item *parent, const std::string &text) -> Text *;

        HBarGraph *bar;
        Text *titleLabel;
        Image *leftArrow;
        Image *rightArrow;
        UpdateCallback updateBarCallback;
        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;
    };
} // namespace gui
