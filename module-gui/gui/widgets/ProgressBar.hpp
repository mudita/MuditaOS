// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

        virtual void setMaximum(unsigned int value)         = 0;
        virtual auto setValue(unsigned int value) -> bool   = 0;
        virtual void setPercentageValue(unsigned int value) = 0;
        [[nodiscard]] virtual int getMaximum() const        = 0;
    };

    class ProgressBar : public Rect, public Progress
    {
      public:
        [[deprecated]] ProgressBar(Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h);

        void setMaximum(unsigned int value) noexcept override;
        auto setValue(unsigned int value) noexcept -> bool override;
        void setPercentageValue(unsigned int value) noexcept override;
        [[nodiscard]] int getMaximum() const noexcept override;

        void buildDrawListImplementation(std::list<Command> &commands) override;
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
        auto setValue(unsigned int value) noexcept -> bool override;
        void setPercentageValue(unsigned int value) noexcept override;
        [[nodiscard]] int getMaximum() const noexcept override;

        void buildDrawListImplementation(std::list<Command> &commands) override;
        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;

      private:
        void createWidgets();

        auto calculateProgressIndicatorCenter() const -> Point;
        auto getPercentageValue() const -> float;

        unsigned int maxValue     = 0U;
        unsigned int currentValue = 0U;
        Arc *progressArc          = nullptr;
        Circle *progressIndicator = nullptr;
    };
} // namespace gui
