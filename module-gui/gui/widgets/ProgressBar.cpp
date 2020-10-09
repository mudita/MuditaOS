#include <module-utils/log/log.hpp>
#include <module-utils/math/Math.hpp>

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

    void ProgressBar::setValue(unsigned int value) noexcept
    {
        currentValue = std::clamp(value, 0U, maxValue);
    }

    void ProgressBar::setPercentageValue(unsigned int value) noexcept
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = std::lround(static_cast<float>(maxValue) * percent);
        setValue(absoluteValue);
    }

    std::list<DrawCommand *> ProgressBar::buildDrawList()
    {
        if (!visible) {
            return {};
        }

        std::list<DrawCommand *> baseCommands = gui::Rect::buildDrawList();
        auto it                               = baseCommands.begin();
        it++;
        CommandRectangle *fill = static_cast<CommandRectangle *>(*it);

        uint32_t progressSize = maxValue == 0U ? 0 : (currentValue * widgetArea.w) / maxValue;
        fill->w               = progressSize;

        std::list<DrawCommand *> commands;
        commands.splice(commands.end(), baseCommands, it);
        commands.splice(commands.end(), baseCommands);
        return commands;
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

    Point CircularProgressBar::calculateProgressIndicatorCenter() const noexcept
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

    void CircularProgressBar::setValue(unsigned int value) noexcept
    {
        currentValue = std::clamp(value, 0U, maxValue);
    }

    void CircularProgressBar::setPercentageValue(unsigned int value) noexcept
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = std::lround(static_cast<float>(maxValue) * percent);
        setValue(absoluteValue);
    }

    float CircularProgressBar::getPercentageValue() const noexcept
    {
        if (maxValue == 0) {
            return .0f;
        }
        return static_cast<float>(currentValue) / maxValue;
    }

    std::list<DrawCommand *> CircularProgressBar::buildDrawList()
    {
        using namespace trigonometry;

        if (!visible) {
            return {};
        }

        progressArc->setSweepAngle(getPercentageValue() * FullAngle);
        progressIndicator->setCenter(calculateProgressIndicatorCenter());
        return Circle::buildDrawList();
    }

    bool CircularProgressBar::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        return true;
    }
} /* namespace gui */
