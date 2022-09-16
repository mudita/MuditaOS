// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        using UpdateCallback = std::function<bool(std::uint8_t)>;

        SpinBox(Item *parent,
                const std::string &title,
                const std::string &titleFocused,
                UpdateCallback updateCallback,
                std::uint8_t maxValue,
                std::uint8_t startValue,
                std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr,
                std::function<void()> navBarRestoreFromTemporaryMode      = nullptr);

      private:
        auto addArrow(Item *parent, const std::string &arrowName, Alignment::Horizontal aligment, bool visible)
            -> Image *;
        auto addBarGraph(Item *parent, std::uint8_t maxValue, std::uint8_t startValue) -> HBarGraph *;
        auto addTitle(Item *parent, const std::string &text) -> Text *;

        HBarGraph *bar    = nullptr;
        Text *titleLabel  = nullptr;
        Image *leftArrow  = nullptr;
        Image *rightArrow = nullptr;
        UpdateCallback updateBarCallback;
        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;
    };
} // namespace gui
