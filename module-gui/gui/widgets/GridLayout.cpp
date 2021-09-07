// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "GridLayout.hpp"
#include "InputEvent.hpp"
#include "Style.hpp"
#include <log.hpp>

using namespace gui;

GridLayout::GridLayout(
    Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid)
    : BoxLayout(parent, x, y, w, h), grid(grid)
{
    setPenWidth(style::window::default_border_no_focus_w);
    setPenFocusWidth(style::window::default_border_no_focus_w);

    this->borderCallback = [this](const InputEvent &inputEvent) -> bool {
        if (!inputEvent.isShortRelease()) {
            return false;
        }

        auto it       = this->getNavigationFocusedItem();
        auto distance = std::distance(children.begin(), it);

        switch (inputEvent.getKeyCode()) {
        case KeyCode::KEY_UP: {
            auto col   = static_cast<uint32_t>(distance % colSize);
            Item *item = getFirstActiveItem(getLastColumnIndex(col), (-1) * static_cast<int>(colSize));
            if (item)
                this->setFocusItem(item);
            return true;
        }
        case KeyCode::KEY_DOWN: {
            auto col   = static_cast<uint32_t>(distance % colSize);
            Item *item = getFirstActiveItem(col, static_cast<int>(colSize));
            if (item)
                this->setFocusItem(item);
            return true;
        }
        case KeyCode::KEY_LEFT: {
            auto row   = static_cast<uint32_t>(distance / colSize);
            Item *item = getFirstActiveItem(getLastRowIndex(row), -1);
            if (item)
                this->setFocusItem(item);
            return true;
        }
        case KeyCode::KEY_RIGHT: {
            auto row   = static_cast<uint32_t>(distance / colSize);
            Item *item = getFirstActiveItem(colSize * row, 1);
            if (item)
                this->setFocusItem(item);
            return true;
        }
        default: {
            return false;
        }
        }
    };
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

    colSize = children.size() < area().w / grid.x ? children.size() : area().w / grid.x;
    rowSize = colSize != 0 ? (children.size() / colSize) : 1;
    if (colSize > 1 && (static_cast<double>(children.size()) / colSize) > 1.0 && (children.size() % colSize)) {
        rowSize++;
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
            (*it)->setNavigationItem(NavigationDirection::LEFT, getFirstActiveItem(i - 1, -1));
        }

        if (it != std::prev(children.end()) && (i + 1) % colSize != 0) {
            (*it)->setNavigationItem(NavigationDirection::RIGHT, getFirstActiveItem(i + 1, 1));
        }

        if ((i - offset) >= 0) {
            (*it)->setNavigationItem(NavigationDirection::UP, getFirstActiveItem(i - offset, (-1) * offset));
        }
        if ((i + offset) < static_cast<int>(children.size())) {
            (*it)->setNavigationItem(NavigationDirection::DOWN, getFirstActiveItem(i + offset, offset));
        }
    }
}

Item *GridLayout::getFirstActiveItem(uint32_t startposition, int step)
{
    Item *retItem = nullptr;
    int index     = static_cast<int>(startposition);
    uint32_t row  = startposition / colSize;
    while (index >= 0 && index < static_cast<int>(children.size())) {
        ///> condition for movement along row (+1,-1 step)
        if ((step == 1 || step == -1) && (index / colSize != row)) {
            break;
        }
        std::list<Item *>::iterator tmpit = std::next(children.begin(), index);
        if ((*tmpit)->isActive()) {
            retItem = *tmpit;
            break;
        }
        index += step;
    }
    return retItem;
}
