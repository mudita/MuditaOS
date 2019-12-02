#include "GridLayout.hpp"
#include <log/log.hpp>
#include "Style.hpp"

using namespace gui;

GridLayout::GridLayout(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, GridSize grid)
    : BoxLayout(parent, x, y, w, h), grid(grid)
{
    setPenWidth(style::window::default_border_no_focus_w);
    setPenFocusWidth(style::window::default_border_focucs_w);
}

void GridLayout::resizeItems()
{

    if( grid.x == 0 || grid.y == 0 ) {
        LOG_ERROR("Grid == 0 - abort");
        return;
    }
    uint32_t el_in_x = widgetArea.w / grid.x;
    uint32_t el_in_y = widgetArea.h / grid.y;
    uint32_t strech_x = 0;
    uint32_t strech_y = 0;
    uint32_t max_elements = el_in_x * el_in_y;

    if (children.size() > max_elements)
    {
        LOG_ERROR("More children than possible to show");
    }
    if (strech) {
        strech_x = (widgetArea.w - grid.x*el_in_x )/el_in_x;
        strech_y = (widgetArea.h - grid.y*el_in_y )/el_in_y;
    }

    int i = 0;
    int row = 0;
    int col = 0;
    for (auto it = children.begin(); it != children.end() && i <= max_elements; ++it, ++i)
    {
        // check if element will fit in
        if ((*it)->widgetArea.w > grid.x || (*it)->widgetArea.h > grid.y)
        {
            LOG_ERROR("Element %d too big", i);
        }
        // set position
        (*it)->setPosition(col * grid.x + strech_x, row * grid.y + strech_y);
        // shift row/col
        ++col;
        if (col == el_in_y)
        {
            col = 0;
            ++row;
        }
    }
}

// TODO commomize - move loop to lambda
void GridLayout::setNavigation(Item *previous, Item *next)
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
            LOG_INFO("Set next navigation for item %d", i);
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
        if (col == el_in_y)
        {
            col = 0;
            ++row;
        }
    }
    // TODO add next item after GridLayout element and previous item after GridLayout element
    // children.front()->setNavigationItem(NavigationDirection::UP | NavigationDirection::LEFT, previous);
    // auto last_element = (*std::next(children.begin(), max_elements < children.size() ? max_elements - 1 : children.size()));
    // last_element->setNavigationItem(NavigationDirection::DOWN | NavigationDirection::RIGHT, next);
}

bool GridLayout::onFocus(bool state)
{
    if(children.size()) {
        children.front()->setFocus(state);
    }
    return true;
}
