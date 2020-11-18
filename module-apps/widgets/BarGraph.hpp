// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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

      public:
        Rect *createRectangle(uint32_t width, uint32_t height);

        Rect *createSpace(uint32_t width, uint32_t height);

        void setPercentageValue(unsigned int value) override;
    };

    class VBarGraph : public VBox, public BarGraph
    {
        void incrementWith(uint32_t levels);
        void decrementWith(uint32_t levels);

      public:
        VBarGraph(Item *parent = nullptr, Position x = 0, Position y = 0, uint32_t numberOfRectangles = 0);
        void setMaximum(unsigned int value) final;
        void setValue(unsigned int value) final;
        void update(int value = 1) final;
    };

    class HBarGraph : public HBox, public BarGraph
    {
        void incrementWith(uint32_t levels);
        void decrementWith(uint32_t levels);

      public:
        HBarGraph(Item *parent = nullptr, Position x = 0, Position y = 0, uint32_t numberOfRectangles = 0);
        void setMaximum(unsigned int value) final;
        void setValue(unsigned int value) final;
        void update(int value = 1) final;
    };

} /* namespace gui */
