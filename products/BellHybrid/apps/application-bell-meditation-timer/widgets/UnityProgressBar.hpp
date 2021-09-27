// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ProgressBar.hpp"

namespace gui
{
    namespace
    {
        inline constexpr auto DEF_FACIA_WIDTH = 4U;
        inline constexpr auto DEF_GAP_WIDTH   = 14U;
    } // namespace

    class UnityProgressBar : public Rect, public Progress
    {
      public:
        UnityProgressBar(Item *parent,
                         std::uint32_t x,
                         std::uint32_t y,
                         std::uint32_t w,
                         std::uint32_t h,
                         std::uint32_t faciaWidth = DEF_FACIA_WIDTH,
                         std::uint32_t gapWidth   = DEF_GAP_WIDTH);

        void setMaximum(unsigned int value) noexcept override;
        auto setValue(unsigned int value) noexcept -> bool override;
        void setPercentageValue(unsigned int value) noexcept override;
        [[nodiscard]] int getMaximum() const noexcept override;

        void buildDrawListImplementation(std::list<Command> &commands) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

        void setFaciaSize(std::uint32_t faciaWidth, std::uint32_t gapWidth);
        void setDisplaySize(std::uint32_t width, std::uint32_t height);

      private:
        std::uint32_t faciaWidth;
        std::uint32_t gapWidth;
        std::uint32_t displayWidth;
        std::uint32_t displayHeight;

        unsigned int maxValue     = 0;
        unsigned int currentValue = 0;
    };

} /* namespace gui */
