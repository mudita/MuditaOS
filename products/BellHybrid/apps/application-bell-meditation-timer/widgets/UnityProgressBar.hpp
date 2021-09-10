// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ProgressBar.hpp"

namespace gui
{
    class UnityProgressBar : public ProgressBar
    {
      public:
        UnityProgressBar(Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h);

        void setMaximum(unsigned int value) noexcept override;
        auto setValue(unsigned int value) noexcept -> bool override;
        void setPercentageValue(unsigned int value) noexcept override;

        void buildDrawListImplementation(std::list<Command> &commands) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      private:
        const unsigned int rw = 2U;
        const unsigned int gw = 8U;

        unsigned int maxValue     = 0U;
        unsigned int currentValue = 0U;
    };

} /* namespace gui */
