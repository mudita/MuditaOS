// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <module-gui/gui/widgets/ProgressBar.hpp>

namespace style::bargraph
{
    constexpr inline auto rect_axis_length_short_medium = 8;
    constexpr inline auto rect_axis_length_long_medium  = 20;
    constexpr inline auto rect_axis_length_short_small  = 3;
    constexpr inline auto rect_axis_length_long_small   = 14;
    constexpr inline auto spacing                       = 17;
    constexpr inline auto radius_medium                 = 4;
    constexpr inline auto radius_small                  = 2;

} // namespace style::bargraph

namespace gui
{
    enum class BarGraphStyle
    {
        Heavy, /// ThickLine style Bars with white/black fill.
        Light, /// SlimLine style Bars with gray/black border fill.
    };

    struct BarStyle
    {
        Length width{0};
        Length height{0};
        Length radius{0};
        Margins margin{0, 0, 0, 0};
        std::function<void(gui::Rect *bar)> fillRender  = nullptr;
        std::function<void(gui::Rect *bar)> emptyRender = nullptr;
    };

    class BarGraph : public Progress
    {
      protected:
        std::vector<gui::Rect *> rectangles;
        std::uint32_t numberOfRectangles;
        std::uint32_t currentLevel = 0;
        BarStyle barStyle;

        [[nodiscard]] auto createRectangle() const -> Rect *;
        void createRectangles();

        virtual void applyBarStyle(BarGraphStyle graphStyle);
        virtual void createGraph(){};

      public:
        void setPercentageValue(unsigned int value) override;

        [[nodiscard]] auto getValue() const -> std::uint32_t
        {
            return currentLevel;
        }

        auto setValue(unsigned int value) -> bool override;
        void setMaximum(unsigned int value) final;
        [[nodiscard]] int getMaximum() const noexcept final;
    };

    class VBarGraph : public VBox, public BarGraph
    {
      public:
        VBarGraph(Item *parent                = nullptr,
                  Position x                  = 0,
                  Position y                  = 0,
                  uint32_t numberOfRectangles = 0,
                  BarGraphStyle graphStyle    = BarGraphStyle::Heavy);
        void applyBarStyle(BarGraphStyle graphStyle) final;
        void createGraph() final;
    };

    class HBarGraph : public HBox, public BarGraph
    {
      public:
        HBarGraph(Item *parent                = nullptr,
                  Position x                  = 0,
                  Position y                  = 0,
                  uint32_t numberOfRectangles = 0,
                  BarGraphStyle graphStyle    = BarGraphStyle::Heavy);
        void applyBarStyle(BarGraphStyle graphStyle) final;
        void createGraph() final;
    };

} /* namespace gui */
