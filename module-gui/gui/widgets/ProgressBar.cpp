// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include <Math.hpp>

#include "DrawCommand.hpp"
#include "ProgressBar.hpp"

namespace gui
{
    ProgressBar::ProgressBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : Rect{parent, x, y, w, h}
    {
        setFillColor(ColorFullBlack);
        setPenWidth(2);
        createWidgets();
        updateDrawArea();
    }

    void ProgressBar::createWidgets()
    {
        // fillRect is smaller, to avoid border overlaping
        fillRect = new gui::Rect(this, 0, 1, widgetArea.w, widgetArea.h - 2);
        fillRect->setRadius(widgetArea.h / 2 - 1);
        fillRect->setFilled(true);
        fillRect->setFillColor(Color{0, 0});
        Rect::setRadius(widgetArea.h / 2);
    }

    void ProgressBar::setMaximum(unsigned int value) noexcept
    {
        maxValue = value;
        if (currentValue > maxValue) {
            currentValue = maxValue;
        }
    }

    bool ProgressBar::setValue(unsigned int value) noexcept
    {
        currentValue = std::clamp(value, 0U, maxValue);
        return currentValue == value;
    }

    void ProgressBar::setPercentageValue(unsigned int value) noexcept
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = std::lround(static_cast<float>(maxValue) * percent);
        setValue(absoluteValue);
    }
    int ProgressBar::getMaximum() const noexcept
    {
        return maxValue;
    }

    void ProgressBar::buildDrawListImplementation(std::list<Command> &commands)
    {
        uint32_t progressSize = maxValue == 0U ? 0 : (currentValue * widgetArea.w) / maxValue;
        drawArea.w            = progressSize;

        gui::Rect::buildDrawListImplementation(commands);
    }

    bool ProgressBar::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        fillRect->setSize(newDim.w, newDim.h);
        return true;
    }

    CircularProgressBar::CircularProgressBar(Item *parent, const Circle::ShapeParams &shape) : Circle{parent, shape}
    {
        createWidgets();
        updateDrawArea();
    }

    void CircularProgressBar::createWidgets()
    {
        const auto progressArcRadius = radius + 1;
        const auto progressArcWidth  = penWidth + 2;
        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(center)
            .setRadius(progressArcRadius)
            .setStartAngle(-90) // Start drawing the circle from top.
            .setSweepAngle(0)
            .setPenWidth(progressArcWidth)
            .setBorderColor(ColorFullBlack);
        progressArc = new Arc(this, arcParams);

        Circle::ShapeParams indicatorParams;
        indicatorParams.setCenterPoint(calculateProgressIndicatorCenter())
            .setRadius(progressArcWidth)
            .setPenWidth(penWidth + 1)
            .setBorderColor(ColorFullBlack)
            .setFillColor(ColorFullBlack);
        progressIndicator = new Circle(this, indicatorParams);
    }

    Point CircularProgressBar::calculateProgressIndicatorCenter() const
    {
        using namespace trigonometry;
        const auto sweepAngleRadians = toRadians(progressArc->getSweepAngle() + progressArc->getStartAngle());
        return Point(center.x + AdjacentSide::fromAngle(sweepAngleRadians, radius - (penWidth / 2)),
                     center.y + OppositeSide::fromAngle(sweepAngleRadians, radius - (penWidth / 2)));
    }

    void CircularProgressBar::setMaximum(unsigned int value) noexcept
    {
        maxValue = value;
        if (currentValue > maxValue) {
            currentValue = maxValue;
        }
    }

    bool CircularProgressBar::setValue(unsigned int value) noexcept
    {
        currentValue = std::clamp(value, 0U, maxValue);
        return value == currentValue;
    }

    void CircularProgressBar::setPercentageValue(unsigned int value) noexcept
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = std::lround(static_cast<float>(maxValue) * percent);
        setValue(absoluteValue);
    }
    int CircularProgressBar::getMaximum() const noexcept
    {
        return maxValue;
    }

    float CircularProgressBar::getPercentageValue() const
    {
        if (maxValue == 0) {
            return .0f;
        }
        return static_cast<float>(currentValue) / maxValue;
    }

    void CircularProgressBar::buildDrawListImplementation(std::list<Command> &commands)
    {
        using namespace trigonometry;

        progressArc->setSweepAngle(std::ceil(getPercentageValue() * FullAngle));
        progressIndicator->setCenter(calculateProgressIndicatorCenter());

        Circle::buildDrawListImplementation(commands);
    }

    bool CircularProgressBar::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        return true;
    }

    ArcProgressBar::ArcProgressBar(Item *parent, const Arc::ShapeParams &shape, ProgressDirection direction)
        : Arc{parent, shape}, direction{direction}
    {
        if (direction == ProgressDirection::CounterClockwise) {
            start -= sweep;
        };
        createWidgets();
        updateDrawArea();
    }

    void ArcProgressBar::createWidgets()
    {
        // Arc progress indicator (stronger line) must be a bit wider than the base circle
        // Those values were selected to match the design and look good enough on multiple
        // radius and penWidth values
        const auto progressArcRadius       = radius + 3;
        const auto progressArcWidth        = penWidth + 7;
        const auto progressIndicatorRadius = (progressArcWidth - 1) / 2;

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(center)
            .setRadius(progressArcRadius)
            .setSweepAngle(0)
            .setPenWidth(progressArcWidth)
            .setBorderColor(ColorFullBlack);

        if (direction == ProgressDirection::Clockwise) {
            arcParams.setStartAngle(start);
        }
        else {
            arcParams.setStartAngle(start + sweep);
        }
        progressArc = new Arc(this, arcParams);

        Circle::ShapeParams indicatorStartParams;
        indicatorStartParams.setCenterPoint(calculateStartIndicatorCenter())
            .setRadius(progressIndicatorRadius)
            .setPenWidth(2)
            .setBorderColor(ColorFullBlack)
            .setFillColor(ColorFullBlack);
        progressStartIndicator = new Circle(this, indicatorStartParams);

        Circle::ShapeParams indicatorEndParams;
        indicatorEndParams.setCenterPoint(calculateEndIndicatorCenter())
            .setRadius(progressIndicatorRadius)
            .setPenWidth(2)
            .setBorderColor(ColorFullBlack)
            .setFillColor(ColorFullBlack);
        progressEndIndicator = new Circle(this, indicatorEndParams);
    }

    Point ArcProgressBar::calculateStartIndicatorCenter() const
    {
        using namespace trigonometry;
        const auto sweepAngleRadians = toRadians(progressArc->getSweepAngle() + progressArc->getStartAngle());
        return Point(center.x + AdjacentSide::fromAngle(sweepAngleRadians, radius - (penWidth / 2)),
                     center.y + OppositeSide::fromAngle(sweepAngleRadians, radius - (penWidth / 2)));
    }

    Point ArcProgressBar::calculateEndIndicatorCenter() const
    {
        using namespace trigonometry;
        const auto sweepAngleRadians = toRadians(progressArc->getStartAngle());
        return Point(center.x + AdjacentSide::fromAngle(sweepAngleRadians, radius - (penWidth / 2)),
                     center.y + OppositeSide::fromAngle(sweepAngleRadians, radius - (penWidth / 2)));
    }

    void ArcProgressBar::setMaximum(unsigned int value) noexcept
    {
        maxValue = value;
        if (currentValue > maxValue) {
            currentValue = maxValue;
        }
    }

    bool ArcProgressBar::setValue(unsigned int value) noexcept
    {
        currentValue = std::clamp(value, 0U, maxValue);
        return value == currentValue;
    }

    void ArcProgressBar::setPercentageValue(unsigned int value) noexcept
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = std::lround(static_cast<float>(maxValue) * percent);
        setValue(absoluteValue);
    }
    int ArcProgressBar::getMaximum() const noexcept
    {
        return maxValue;
    }

    float ArcProgressBar::getPercentageValue() const
    {
        if (maxValue == 0) {
            return .0f;
        }
        return static_cast<float>(currentValue) / maxValue;
    }

    void ArcProgressBar::buildDrawListImplementation(std::list<Command> &commands)
    {
        if (direction == ProgressDirection::Clockwise) {
            progressArc->setSweepAngle(std::ceil(getPercentageValue() * sweep));
        }
        else {
            progressArc->setStartAngle(start + sweep -
                                       std::ceil(getPercentageValue() * sweep)); // Start drawing the circle from top.
            progressArc->setSweepAngle(std::ceil(getPercentageValue() * sweep));
        }
        progressStartIndicator->setCenter(calculateStartIndicatorCenter());
        progressEndIndicator->setCenter(calculateEndIndicatorCenter());

        Arc::buildDrawListImplementation(commands);
    }

    bool ArcProgressBar::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        return true;
    }

} /* namespace gui */
