/*
 * ListItemProvider.cpp
 *
 *  Created on: 11 mar 2019
 *      Author: robert
 */

#include "ListItemProvider.hpp"

namespace gui
{

    ListItemProvider::ListItemProvider()
    {}

    ListItemProvider::~ListItemProvider()
    {}

    ListItem *ListItemProvider::getItem(
        int index, int firstElement, int prevElement, uint32_t count, int remaining, bool topDown)
    {
        return nullptr;
    }

    void ListItemProvider::requestDBUpdate(const uint32_t offset, const uint32_t limit)
    {}

} /* namespace gui */
