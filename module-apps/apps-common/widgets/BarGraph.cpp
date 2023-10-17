// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BarGraph.hpp"

namespace gui
{
    static inline auto rectAxisLengthFrom(uint32_t numberOfRectangles) -> uint32_t
    {
        return numberOfRectangles * (style::bargraph::spacing + style::bargraph::rect_axis_length_short_medium) -
               style::bargraph::spacing;
    }

    void BarGraph::setPercentageValue(unsigned int value)
    {
        const auto percent       = static_cast<float>(value) / 100.0f;
        const auto absoluteValue = numberOfRectangles * percent;
        setValue(absoluteValue);
    }

    auto BarGraph::createRectangle() const -> Rect *
    {
        auto rectangle = new Rect(nullptr, 0, 0, 0, 0);
        rectangle->setMinimumSize(barStyle.width, barStyle.height);
        rectangle->setFillColor(ColorFullBlack);
        rectangle->setBorderColor(ColorFullBlack);
        rectangle->setFilled(false);
        rectangle->setRadius(barStyle.radius);
        rectangle->setPenWidth(style::window::default_border_focus_w);
        return rectangle;
    }

    void BarGraph::createRectangles()
    {
        for (std::uint32_t i = 0; i < numberOfRectangles; i++) {

            auto rectangle = createRectangle();

            if ((i + 1) != numberOfRectangles) {
                rectangle->setMargins(barStyle.margin);
            }

            rectangles.push_back(rectangle);
        }
    }

    bool BarGraph::setValue(unsigned int value)
    {
        if (value > numberOfRectangles) {
            LOG_ERROR("Provider value greater than graph scale");
            return false;
        }

        currentLevel = value;
        for (std::uint32_t i = 0; i < currentLevel; i++) {
            barStyle.fillRender(rectangles[i]);
        }
        for (std::uint32_t i = currentLevel; i < numberOfRectangles; i++) {
            barStyle.emptyRender(rectangles[i]);
        }

        return true;
    }

    void BarGraph::setMaximum(unsigned int value)
    {
        numberOfRectangles = value;

        if (currentLevel > numberOfRectangles) {
            currentLevel = numberOfRectangles;
        }
    }
    int BarGraph::getMaximum() const noexcept
    {
        return numberOfRectangles;
    }

    void BarGraph::applyBarStyle(BarGraphStyle graphStyle)
    {
        switch (graphStyle) {
        case BarGraphStyle::Heavy:
            barStyle.radius      = style::bargraph::radius_medium;
            barStyle.fillRender  = [](gui::Rect *bar) { bar->setFillColor(ColorFullBlack); };
            barStyle.emptyRender = [](gui::Rect *bar) { bar->setFillColor(ColorFullWhite); };
            break;
        case BarGraphStyle::Light:
            barStyle.radius      = style::bargraph::radius_small;
            barStyle.fillRender  = [](gui::Rect *bar) { bar->setBorderColor(ColorFullBlack); };
            barStyle.emptyRender = [](gui::Rect *bar) { bar->setBorderColor(ColorGrey); };
            break;
        }
    }

    VBarGraph::VBarGraph(Item *parent, Position x, Position y, uint32_t numberOfRectangles, BarGraphStyle graphStyle)
        : VBox(parent, x, y, style::bargraph::rect_axis_length_long_medium, rectAxisLengthFrom(numberOfRectangles))
    {
        applyBarStyle(graphStyle);
        setMinimumSize(style::bargraph::rect_axis_length_long_medium, rectAxisLengthFrom(numberOfRectangles));
        setEdges(RectangleEdge::None);

        setMaximum(numberOfRectangles);
        createGraph();

        std::reverse(std::begin(rectangles), std::end(rectangles));
    }

    void VBarGraph::createGraph()
    {
        if (!rectangles.empty()) {
            erase();
            rectangles.clear();
        }

        setMinimumHeight(rectAxisLengthFrom(numberOfRectangles));
        createRectangles();

        for (auto rect : rectangles) {
            addWidget(rect);
        }

        resizeItems();
    }

    void VBarGraph::applyBarStyle(BarGraphStyle graphStyle)
    {
        BarGraph::applyBarStyle(graphStyle);

        switch (graphStyle) {
        case BarGraphStyle::Heavy:
            barStyle.width  = style::bargraph::rect_axis_length_long_medium;
            barStyle.height = style::bargraph::rect_axis_length_short_medium;
            barStyle.margin = Margins(0, 0, 0, style::bargraph::spacing);
            break;
        case BarGraphStyle::Light:
            barStyle.width  = style::bargraph::rect_axis_length_long_small;
            barStyle.height = style::bargraph::rect_axis_length_short_small;
            barStyle.margin = Margins(0, 0, 0, style::bargraph::spacing);
            break;
        }
    }

    HBarGraph::HBarGraph(Item *parent, Position x, Position y, uint32_t numberOfRectangles, BarGraphStyle graphStyle)
        : HBox(parent, x, y, rectAxisLengthFrom(numberOfRectangles), style::bargraph::rect_axis_length_long_medium)
    {
        applyBarStyle(graphStyle);
        setMinimumSize(rectAxisLengthFrom(numberOfRectangles), style::bargraph::rect_axis_length_long_medium);
        setEdges(RectangleEdge::None);

        setMaximum(numberOfRectangles);
        createGraph();
    }

    void HBarGraph::createGraph()
    {
        if (!rectangles.empty()) {
            erase();
            rectangles.clear();
        }

        setMinimumWidth(rectAxisLengthFrom(numberOfRectangles));
        createRectangles();

        for (auto rect : rectangles) {
            addWidget(rect);
        }

        resizeItems();
    }

    void HBarGraph::applyBarStyle(BarGraphStyle graphStyle)
    {
        BarGraph::applyBarStyle(graphStyle);

        switch (graphStyle) {
        case BarGraphStyle::Heavy:
            barStyle.width  = style::bargraph::rect_axis_length_short_medium;
            barStyle.height = style::bargraph::rect_axis_length_long_medium;
            barStyle.margin = Margins(0, 0, style::bargraph::spacing, 0);
            break;
        case BarGraphStyle::Light:
            barStyle.width  = style::bargraph::rect_axis_length_short_small;
            barStyle.height = style::bargraph::rect_axis_length_long_small;
            barStyle.margin = Margins(0, 0, style::bargraph::spacing, 0);
            break;
        }
    }
} /* namespace gui */
