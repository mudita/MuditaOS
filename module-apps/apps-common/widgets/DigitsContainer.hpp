// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>

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

    struct DigitsContainer
    {
        explicit DigitsContainer(std::uint32_t digitsCount);
        std::vector<Label *> digits;

        void setMinutesBox(std::uint32_t minutes, DimensionsParams params);
        void setSecondsBox(std::uint32_t seconds, DimensionsParams params);
    };

} // namespace gui
