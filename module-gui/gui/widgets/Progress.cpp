/*
 * Progress.cpp
 *
 *  Created on: 22 mar 2019
 *      Author: robert
 */

#include "Progress.hpp"

namespace gui
{

    Progress::Progress() : Rect()
    {
        createWidgets();
    }

    Progress::Progress(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h)
        : Rect(parent, x, y, w, h), total{0}, current{0}
    {
        setFillColor(Color{0, 0});
        setPenWidth(2);
        createWidgets();
        updateDrawArea();
    }

    void Progress::createWidgets()
    {

        // fillRect is smaller, to avoid border overlaping
        fillRect = new gui::Rect(this, 0, 1, widgetArea.w, widgetArea.h - 2);
        fillRect->setRadius(widgetArea.h / 2 - 1);
        fillRect->setFilled(true);
        fillRect->setFillColor(Color{0, 0});
        Rect::setRadius(widgetArea.h / 2);
    }

    void Progress::setTotalProgress(int value)
    {
        if (value < 0)
            value = 0;

        total = value;

        if (current > total)
            current = total;
    }

    void Progress::setCurrentProgress(int value)
    {
        if (value < 0)
            value = 0;
        if (value > total)
            value = total;
        current = value;
    }

    void Progress::setCurrentPercent(int value)
    {
        int newValue = static_cast<int>((float)total * (float)value / 100.0);
        setCurrentProgress(newValue);
    }

    std::list<DrawCommand *> Progress::buildDrawList()
    {

        std::list<DrawCommand *> commands;

        if (visible == false) {
            return commands;
        }

        std::list<DrawCommand *> baseCommands = gui::Rect::buildDrawList();
        auto it                               = baseCommands.begin();

        it++;
        CommandRectangle *fill = reinterpret_cast<CommandRectangle *>(*it);

        uint32_t progressSize = 0;
        progressSize          = (current * widgetArea.w) / total;
        fill->w               = progressSize;

        commands.splice(commands.end(), baseCommands, it);
        commands.splice(commands.end(), baseCommands);

        return commands;
    }

    bool Progress::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {

        fillRect->setSize(newDim.w, newDim.h);
        return true;
    }

} /* namespace gui */
