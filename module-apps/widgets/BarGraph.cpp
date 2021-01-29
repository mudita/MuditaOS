// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

    auto BarGraph::createRectangle(uint32_t width, uint32_t height) const -> Rect *
    {
        auto rectangle = new Rect(nullptr, 0, 0, 0, 0);
        rectangle->setMinimumSize(width, height);
        rectangle->setFillColor(ColorFullBlack);
        rectangle->setBorderColor(ColorFullBlack);
        rectangle->setFilled(false);
        rectangle->setRadius(style::bargraph::radius);
        rectangle->setPenWidth(style::window::default_border_focus_w);
        return rectangle;
    }

    auto BarGraph::incrementWith(uint32_t levels) -> bool
    {
        if ((currentLevel + levels) <= numberOfRectangles) {
            for (uint32_t i = 0; i < levels; ++i) {
                rectangles[currentLevel]->setFillColor(ColorFullBlack);
                rectangles[currentLevel]->setBorderColor(ColorFullBlack);
                ++currentLevel;
            }
            return true;
        }
        else {
            LOG_ERROR("bargraph incremented out of size");
            return false;
        }
    }

    auto BarGraph::decrementWith(uint32_t levels) -> bool
    {
        if (currentLevel >= levels) {
            for (uint32_t i = levels; i > 0; --i) {
                --currentLevel;
                rectangles[currentLevel]->setFillColor(ColorFullWhite);
                rectangles[currentLevel]->setBorderColor(ColorFullBlack);
            }
            return true;
        }
        else {
            LOG_ERROR("bargraph incremented out of size");
            return false;
        }
    }

    auto BarGraph::update(int value) -> bool
    {
        if (value > 0) {
            return incrementWith(value);
        }
        else if (value < 0) {
            return decrementWith((-value));
        }

        return false;
    }

    bool BarGraph::setValue(unsigned int value)
    {
        if (const auto levels = static_cast<int>(value) - static_cast<int>(currentLevel); levels > 0) {
            return incrementWith(levels);
        }
        else if (levels < 0) {
            return decrementWith(-levels);
        }
        return false;
    }

    VBarGraph::VBarGraph(Item *parent, Position x, Position y, uint32_t numberOfRectangles)
        : VBox(parent, x, y, style::bargraph::rect_axis_length_lrg, rectAxisLenghtFrom(numberOfRectangles))
    {
        setMinimumSize(style::bargraph::rect_axis_length_lrg, rectAxisLenghtFrom(numberOfRectangles));
        setEdges(RectangleEdge::None);
        setMaximum(numberOfRectangles);
        std::reverse(std::begin(rectangles), std::end(rectangles));
    }

    void VBarGraph::setMaximum(unsigned int value)
    {
        numberOfRectangles = value;

        if (currentLevel > numberOfRectangles) {
            currentLevel = numberOfRectangles;
        }
        if (!rectangles.empty()) {
            erase();
            rectangles.clear();
        }

        for (unsigned int i = 0; i <= numberOfRectangles; i++) {

            auto rectangle =
                createRectangle(style::bargraph::rect_axis_length_lrg, style::bargraph::rect_axis_length_sml);

            if ((i + 1) != numberOfRectangles) {
                rectangle->setMargins(Margins(0, 0, 0, style::bargraph::spacing));
            }

            addWidget(rectangle);
            rectangles.push_back(rectangle);
        }
    }

    HBarGraph::HBarGraph(Item *parent, Position x, Position y, uint32_t numberOfRectangles) : HBox(parent)
    {
        setMinimumSize(rectAxisLenghtFrom(numberOfRectangles), style::bargraph::rect_axis_length_lrg);
        setEdges(RectangleEdge::None);
        setMaximum(numberOfRectangles);
    }

    void HBarGraph::setMaximum(unsigned int value)
    {
        numberOfRectangles = value;
        if (currentLevel > numberOfRectangles) {
            currentLevel = numberOfRectangles;
        }

        if (!rectangles.empty()) {
            erase();
            rectangles.clear();
        }

        for (unsigned int i = 0; i <= numberOfRectangles; i++) {

            auto rectangle =
                createRectangle(style::bargraph::rect_axis_length_sml, style::bargraph::rect_axis_length_lrg);

            if ((i + 1) != numberOfRectangles) {
                rectangle->setMargins(Margins(0, 0, style::bargraph::spacing, 0));
            }

            addWidget(rectangle);
            rectangles.push_back(rectangle);
        }
    }

} /* namespace gui */
