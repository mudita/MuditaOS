#include "GridLayout.hpp"
#include "Style.hpp"
#include <log/log.hpp>

using namespace gui;

GridLayout::GridLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid)
    : BoxLayout(parent, x, y, w, h), grid(grid)
{
    setPenWidth(style::window::default_border_no_focus_w);
    setPenFocusWidth(style::window::default_border_no_focus_w);
}

void GridLayout::resizeItems()
{

    if (grid.x == 0 || grid.y == 0)
    {
        LOG_ERROR("Grid == 0 - abort");
        return;
    }
    uint32_t el_in_x = area().w / grid.x;
    uint32_t el_in_y = area().h / grid.y;
    uint32_t strech_x = 0;
    uint32_t strech_y = 0;
    uint32_t max_elements = el_in_x * el_in_y;

    if (children.size() > max_elements)
    {
        LOG_ERROR("More children than possible to show");
        return;
    }
    if (el_in_x > 2)
        strech_x = (area().w - grid.x * el_in_x) / (el_in_x - 1);
    if (el_in_y > 2)
        strech_y = (area().h - grid.y * el_in_y) / (el_in_y - 1);

    int row = 0;
    int col = 0;
    for (auto it : children)
    {
        // check if element will fit in
        if (it->area().w > grid.x || it->area().h > grid.y)
        {
            continue;
        }
        // set position
        it->area().pos(Axis::X) = col * (grid.x + strech_x) + (grid.x - it->area().w) / 2;
        it->area().pos(Axis::Y) = row * (grid.y + strech_y) + (grid.y - it->area().h) / 2;
        // shift row/col
        ++col;
        if (col == el_in_x)
        {
            col = 0;
            ++row;
        }
    }
}

// TODO commomize - move loop to lambda
void GridLayout::setNavigation()
{
    uint32_t el_in_x = widgetArea.w / grid.x;
    uint32_t el_in_y = widgetArea.h / grid.y;
    uint32_t max_elements = el_in_x * el_in_y;
    int i = 0;
    int row = 0;
    int col = 0;
    if (children.size() == 0)
    {
        LOG_ERROR("No children to set navigation");
    }
    for (auto it = children.begin(); it != children.end() && i < max_elements; ++it, ++i)
    {
        if (i != 0)
        {
            (*it)->setNavigationItem(NavigationDirection::LEFT, *std::prev(it));
        }
        if (col + 1 % el_in_x != 0 && i + 1 < children.size())
        {
            (*it)->setNavigationItem(NavigationDirection::RIGHT, *std::next(it));
        }
        if (row != 0)
        {
            (*it)->setNavigationItem(NavigationDirection::UP, *std::prev(it, el_in_x));
        }
        if (row + 1 % el_in_y != 0 && i + el_in_x < children.size())
        {
            (*it)->setNavigationItem(NavigationDirection::DOWN, *std::next(it, el_in_x));
        }

        ++col;
        if (col == el_in_x)
        {
            col = 0;
            ++row;
        }
    }
}
