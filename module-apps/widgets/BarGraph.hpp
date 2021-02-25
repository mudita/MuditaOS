// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <module-gui/gui/widgets/ProgressBar.hpp>

namespace style::bargraph
{
    constexpr inline auto rect_axis_length_sml = 8;
    constexpr inline auto rect_axis_length_lrg = 20;
    constexpr inline auto spacing              = 17;
    constexpr inline auto radius               = 4;
} // namespace style::bargraph

namespace gui
{
    class BarGraph : public Progress
    {
      protected:
        std::vector<gui::Rect *> rectangles;
        uint32_t numberOfRectangles;
        uint32_t currentLevel = 0;

        [[nodiscard]] auto createRectangle(uint32_t width, uint32_t height) const -> Rect *;

      public:
        void setPercentageValue(unsigned int value) override;

        [[nodiscard]] auto getValue() const -> uint32_t
        {
            return currentLevel;
        }

        auto setValue(unsigned int value) -> bool override;
    };

    class VBarGraph : public VBox, public BarGraph
    {
      public:
        VBarGraph(Item *parent = nullptr, Position x = 0, Position y = 0, uint32_t numberOfRectangles = 0);
        void setMaximum(unsigned int value) final;
    };

    class HBarGraph : public HBox, public BarGraph
    {
      public:
        HBarGraph(Item *parent = nullptr, Position x = 0, Position y = 0, uint32_t numberOfRectangles = 0);
        void setMaximum(unsigned int value) final;
    };

} /* namespace gui */
