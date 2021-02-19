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

    auto BarGraph::createRectangle(std::uint32_t width, std::uint32_t height) const -> Rect *
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

    bool BarGraph::setValue(unsigned int value)
    {
        if (value > numberOfRectangles) {
            LOG_ERROR("Provider value greater than graph scale");
            return false;
        }

        currentLevel = value;
        for (std::uint32_t i = 0; i < currentLevel; i++) {
            rectangles[i]->setFillColor(ColorFullBlack);
        }
        for (std::uint32_t i = currentLevel; i < numberOfRectangles; i++) {
            rectangles[i]->setFillColor(ColorFullWhite);
        }

        return true;
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

        for (std::uint32_t i = 0; i < numberOfRectangles; i++) {

            auto rectangle =
                createRectangle(style::bargraph::rect_axis_length_lrg, style::bargraph::rect_axis_length_sml);
            rectangle->setBorderColor(ColorFullBlack);

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

        for (std::uint32_t i = 0; i <= numberOfRectangles; i++) {

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
