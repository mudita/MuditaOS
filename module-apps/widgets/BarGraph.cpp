// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BarGraph.hpp"

namespace gui
{
    static inline auto rectAxisLenghtFrom(uint32_t numberOfRectangles) -> uint32_t
    {
        return numberOfRectangles * (style::bargraph::spacing + style::bargraph::rect_axis_length_sml) -
               style::bargraph::spacing;
    }

    void BarGraph::setPercentageValue(unsigned int value)
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = numberOfRectangles * percent;
        setValue(absoluteValue);
    }

    Rect *BarGraph::createRectangle(uint32_t width, uint32_t height)
    {
        auto rectangle = new Rect(nullptr, 0, 0, width, height);
        rectangle->setFillColor(ColorFullBlack);
        rectangle->setBorderColor(ColorFullBlack);
        rectangle->setFilled(true);
        rectangle->setRadius(style::bargraph::radius);
        rectangle->setPenWidth(style::window::default_border_focus_w);
        return rectangle;
    }

    Rect *BarGraph::createSpace(uint32_t width, uint32_t height)
    {
        auto space = new Rect(nullptr, 0, 0, width, height);
        space->setEdges(RectangleEdge::None);
        return space;
    }

    VBarGraph::VBarGraph(Item *parent, Position x, Position y, uint32_t numberOfRectangles)
        : VBox(parent, x, y, style::bargraph::rect_axis_length_lrg, rectAxisLenghtFrom(numberOfRectangles))
    {
        setRadius(style::bargraph::radius);
        setEdges(RectangleEdge::None);
        setMaximum(numberOfRectangles);
        std::reverse(std::begin(rectangles), std::end(rectangles));
    }

    void VBarGraph::setMaximum(unsigned int value)
    {
        numberOfRectangles = value;
        setSize(rectAxisLenghtFrom(numberOfRectangles), Axis::Y);
        if (currentLevel > numberOfRectangles) {
            currentLevel = numberOfRectangles;
        }
        if (!rectangles.empty()) {
            erase();
            rectangles.clear();
        }
        for (uint32_t i = 0; i < numberOfRectangles; ++i) {
            auto rectangle =
                createRectangle(style::bargraph::rect_axis_length_lrg, style::bargraph::rect_axis_length_sml);
            addWidget(rectangle);
            rectangles.push_back(rectangle);
            addWidget(createSpace(style::bargraph::rect_axis_length_lrg, style::bargraph::spacing));
        }
    }

    void VBarGraph::update(int value)
    {
        if (value > 0) {
            incrementWith(value);
        }
        else if (value < 0) {
            decrementWith((-value));
        }
    }

    void VBarGraph::setValue(unsigned int value)
    {
        if (const auto levels = static_cast<int>(value) - static_cast<int>(currentLevel); levels > 0) {
            incrementWith(levels);
        }
        else if (levels < 0) {
            decrementWith(-levels);
        }
    }

    void VBarGraph::incrementWith(uint32_t levels)
    {
        if ((currentLevel + levels) <= numberOfRectangles) {
            for (uint32_t i = 0; i < levels; ++i) {
                rectangles[currentLevel]->setFillColor(ColorFullBlack);
                rectangles[currentLevel]->setBorderColor(ColorFullBlack);
                ++currentLevel;
            }
        }
        else {
            LOG_ERROR("bargraph incremented out of size");
        }
    }

    void VBarGraph::decrementWith(uint32_t levels)
    {
        if (currentLevel >= levels) {
            for (uint32_t i = levels; i > 0; --i) {
                --currentLevel;
                rectangles[currentLevel]->setFillColor(ColorTray);
                rectangles[currentLevel]->setBorderColor(ColorTray);
            }
        }
        else {
            LOG_ERROR("bargraph incremented out of size");
        }
    }

    HBarGraph::HBarGraph(Item *parent, Position x, Position y, uint32_t numberOfRectangles)
        : HBox(parent, x, y, rectAxisLenghtFrom(numberOfRectangles), style::bargraph::rect_axis_length_lrg)
    {
        setRadius(style::bargraph::radius);
        setEdges(RectangleEdge::None);
        setMaximum(numberOfRectangles);
    }

    void HBarGraph::setMaximum(unsigned int value)
    {
        numberOfRectangles = value;
        setSize(rectAxisLenghtFrom(numberOfRectangles), Axis::X);
        if (currentLevel > numberOfRectangles) {
            currentLevel = numberOfRectangles;
        }
        if (!rectangles.empty()) {
            erase();
            rectangles.clear();
        }
        for (uint32_t i = 0; i < numberOfRectangles; ++i) {
            auto rectangle =
                createRectangle(style::bargraph::rect_axis_length_sml, style::bargraph::rect_axis_length_lrg);
            addWidget(rectangle);
            rectangles.push_back(rectangle);
            addWidget(createSpace(style::bargraph::spacing, style::bargraph::rect_axis_length_lrg));
        }
    }

    void HBarGraph::update(int value)
    {
        if (value > 0) {
            incrementWith(value);
        }
        else if (value < 0) {
            decrementWith(-value);
        }
    }

    void HBarGraph::setValue(unsigned int value)
    {
        if (const auto levels = static_cast<int>(value) - static_cast<int>(currentLevel); levels > 0) {
            incrementWith(levels);
        }
        else if (levels < 0) {
            decrementWith(levels * -1);
        }
    }

    void HBarGraph::incrementWith(uint32_t levels)
    {
        if ((currentLevel + levels) <= numberOfRectangles) {
            for (uint32_t i = 0; i < levels; ++i) {
                rectangles[currentLevel]->setFillColor(ColorFullBlack);
                rectangles[currentLevel]->setBorderColor(ColorFullBlack);
                ++currentLevel;
            }
        }
        else {
            LOG_ERROR("bargraph incremented out of size");
        }
    }

    void HBarGraph::decrementWith(uint32_t levels)
    {
        if (currentLevel >= levels) {
            for (uint32_t i = levels; i > 0; --i) {
                --currentLevel;
                rectangles[currentLevel]->setFillColor(ColorFullWhite);
                rectangles[currentLevel]->setBorderColor(ColorFullBlack);
            }
        }
        else {
            LOG_ERROR("bargraph incremented out of size");
        }
    }

} /* namespace gui */
