#pragma once

#include <cstdint>

#include "Arc.hpp"
#include "Circle.hpp"
#include "Rect.hpp"

namespace gui
{
    class Progress
    {
      public:
        virtual ~Progress() noexcept = default;

        virtual void setMaximum(unsigned int value) noexcept         = 0;
        virtual void setValue(unsigned int value) noexcept           = 0;
        virtual void setPercentageValue(unsigned int value) noexcept = 0;
    };

    class ProgressBar : public Rect, public Progress
    {
      public:
        ProgressBar(Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h);

        void setMaximum(unsigned int value) noexcept override;
        void setValue(unsigned int value) noexcept override;
        void setPercentageValue(unsigned int value) noexcept override;

        std::list<DrawCommand *> buildDrawList() override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      private:
        void createWidgets();

        unsigned int maxValue     = 0U;
        unsigned int currentValue = 0U;
        gui::Rect *fillRect       = nullptr;
    };

    class CircularProgressBar : public Circle, public Progress
    {
      public:
        CircularProgressBar(Item *parent, const Circle::ShapeParams &shape);

        void setMaximum(unsigned int value) noexcept override;
        void setValue(unsigned int value) noexcept override;
        void setPercentageValue(unsigned int value) noexcept override;

        auto buildDrawList() -> std::list<DrawCommand *> override;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;

      private:
        void createWidgets();

        auto calculateProgressIndicatorCenter() const noexcept -> Point;
        auto getPercentageValue() const noexcept -> float;

        unsigned int maxValue     = 0U;
        unsigned int currentValue = 0U;
        Arc *progressArc          = nullptr;
        Circle *progressIndicator = nullptr;
    };
} // namespace gui
