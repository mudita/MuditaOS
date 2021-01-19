// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GridLayout.hpp"
#include "InputEvent.hpp"
#include "Style.hpp"
#include <log/log.hpp>

using namespace gui;

GridLayout::GridLayout(
    Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid)
    : BoxLayout(parent, x, y, w, h), grid(grid)
{
    setPenWidth(style::window::default_border_no_focus_w);
    setPenFocusWidth(style::window::default_border_no_focus_w);

    this->borderCallback = [this](const InputEvent &inputEvent) -> bool {
        if (inputEvent.state != InputEvent::State::keyReleasedShort) {
            return false;
        }

        auto it       = this->getNavigationFocusedItem();
        auto distance = std::distance(children.begin(), it);
        switch (inputEvent.keyCode) {
        case KeyCode::KEY_UP: {
            auto realRowSize = calculateRowSizeForBorderTransition(distance);
            this->setFocusItem((*std::next(it, (realRowSize - 1) * this->colSize)));
            return true;
        }
        case KeyCode::KEY_DOWN: {
            auto realRowSize = calculateRowSizeForBorderTransition(distance);
            this->setFocusItem((*std::prev(it, (realRowSize - 1) * this->colSize)));
            return true;
        }
        case KeyCode::KEY_LEFT: {
            auto realColSize = calculateColumnSizeForBorderTransition(distance);
            this->setFocusItem((*std::next(it, realColSize - 1)));
            return true;
        }
        case KeyCode::KEY_RIGHT: {
            auto realColSize = calculateColumnSizeForBorderTransition(distance);
            this->setFocusItem((*std::prev(it, realColSize - 1)));
            return true;
        }
        default: {
            return false;
        }
        }
    };
}

uint32_t GridLayout::calculateColumnSizeForBorderTransition(const uint32_t currentPosition)
{
    auto realColSize = colSize;
    if (elementsInIncompletedLastRow) {
        if (((currentPosition / colSize) + 1) >= rowSize)
            realColSize = elementsInIncompletedLastRow;
    }
    return realColSize;
}
uint32_t GridLayout::calculateRowSizeForBorderTransition(const uint32_t currentPosition)
{
    auto realRowSize = rowSize;
    if (elementsInIncompletedLastRow) {
        if (((currentPosition % (colSize)) + 1) > elementsInIncompletedLastRow)
            realRowSize--;
    }
    return realRowSize;
}

void GridLayout::handleItemsOutOfGridLayoutArea(uint32_t maxItemsInArea)
{
    for (auto i = maxItemsInArea; i < children.size(); i++) {
        auto it = std::next(children.begin(), i);
        if ((*it)->visible) {
            addToOutOfDrawAreaList(*it);
        }
    }
}

void GridLayout::resizeItems()
{
    if (grid.x == 0 || grid.y == 0) {
        LOG_ERROR("Grid == 0 - abort");
        return;
    }
    uint32_t el_in_x = area().w / grid.x;
    uint32_t el_in_y = area().h / grid.y;

    elementsInIncompletedLastRow = 0;
    colSize = children.size() < area().w / grid.x ? children.size() : area().w / grid.x;
    rowSize                      = colSize != 0 ? (children.size() / colSize) : 1;
    if (colSize > 1 && (static_cast<double>(children.size()) / colSize) > 1.0) {
        elementsInIncompletedLastRow = children.size() % colSize;
    }
    if (elementsInIncompletedLastRow > 0) {
        rowSize++;
    }

    uint32_t strech_x     = 0;
    uint32_t strech_y     = 0;
    uint32_t max_elements = el_in_x * el_in_y;
    /// cant show elements when size is not set
    if (area().w == 0 || area().h == 0) {
        return;
    }

    if (children.size() > max_elements) {
        LOG_ERROR("More children than possible to show: %u > %" PRIu32 "",
                  static_cast<unsigned int>(children.size()),
                  max_elements);
        handleItemsOutOfGridLayoutArea(max_elements);
        return;
    }
    if (el_in_x > 2)
        strech_x = (area().w - grid.x * el_in_x) / (el_in_x - 1);
    if (el_in_y > 2)
        strech_y = (area().h - grid.y * el_in_y) / (el_in_y - 1);

    int row          = 0;
    unsigned int col = 0;
    for (auto it : children) {
        // check if element will fit in
        if (it->area().w > grid.x || it->area().h > grid.y) {
            continue;
        }
        it->setPosition(col * (grid.x + strech_x) + (grid.x - it->area().w) / 2,
                        row * (grid.y + strech_y) + (grid.y - it->area().h) / 2);
        // shift row/col
        ++col;
        if (col == el_in_x) {
            col = 0;
            ++row;
        }
    }
}

// TODO commomize - move loop to lambda
void GridLayout::setNavigation()
{
    int i      = 0;
    int offset = colSize;

    if (children.empty()) {
        return;
    }

    for (auto it = children.begin(); it != children.end(); ++it, ++i) {

        if (it != children.begin() && (i + 1) % colSize != 1) {
            (*it)->setNavigationItem(NavigationDirection::LEFT, nextNavigationItem(std::prev(it)));
        }

        if (it != std::prev(children.end()) && (i + 1) % colSize != 0) {
            (*it)->setNavigationItem(NavigationDirection::RIGHT, nextNavigationItem(std::next(it)));
        }

        if ((i - offset) >= 0) {
            (*it)->setNavigationItem(NavigationDirection::UP, nextNavigationItem(std::prev(it, offset)));
        }
        if ((i + offset) < static_cast<int>(children.size())) {
            (*it)->setNavigationItem(NavigationDirection::DOWN, nextNavigationItem(std::next(it, offset)));
        }
    }
}

Item *GridLayout::nextNavigationItem(std::list<Item *>::iterator it)
{
    if (it != this->children.end() && (*it)->visible && (*it)->activeItem) {
        return *it;
    }
    else {
        return nullptr;
    }
}
