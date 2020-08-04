#include "GridLayout.hpp"
#include "Style.hpp"
#include <log/log.hpp>

using namespace gui;

GridLayout::GridLayout(
    Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid)
    : BoxLayout(parent, x, y, w, h), grid(grid)
{
    setPenWidth(style::window::default_border_no_focus_w);
    setPenFocusWidth(style::window::default_border_no_focus_w);
}

void GridLayout::resizeItems()
{
    if (grid.x == 0 || grid.y == 0) {
        LOG_ERROR("Grid == 0 - abort");
        return;
    }

    rowSize = area().w / grid.x;
    colSize = area().h / grid.y;

    uint32_t strech_x     = 0;
    uint32_t strech_y     = 0;
    uint32_t max_elements = rowSize * colSize;
    /// cant show elements when size is not set
    if (area().w == 0 || area().h == 0) {
        return;
    }

    if (children.size() > max_elements) {
        LOG_ERROR("More children than possible to show: %u > %" PRIu32 "",
                  static_cast<unsigned int>(children.size()),
                  max_elements);
        return;
    }
    if (rowSize > 2)
        strech_x = (area().w - grid.x * rowSize) / (rowSize - 1);
    if (colSize > 2)
        strech_y = (area().h - grid.y * colSize) / (colSize - 1);

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
        if (col == rowSize) {
            col = 0;
            ++row;
        }
    }
}

// TODO commomize - move loop to lambda
void GridLayout::setNavigation()
{
    int i       = 0;
    int offset  = rowSize;

    if (children.size() == 0) {
        LOG_ERROR("No children to set navigation");
    }

    for (auto it = children.begin(); it != children.end(); ++it, ++i) {

        if (it != children.begin() && (i + 1) % rowSize != 1) {
            (*it)->setNavigationItem(NavigationDirection::LEFT, nextNavigationItem(std::prev(it)));
        }

        if (it != std::prev(children.end()) && (i + 1) % rowSize != 0) {
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
