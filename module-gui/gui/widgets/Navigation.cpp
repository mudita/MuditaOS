// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * Navigation.cpp
 *
 *  Created on: 15 mar 2019
 *      Author: robert
 */

#include "Navigation.hpp"
#include "Item.hpp"

namespace gui
{

    void Navigation::setDirectionItem(NavigationDirection direction, Item *item)
    {
        switch (direction) {
        case NavigationDirection::LEFT:
            left = item;
            break;
        case NavigationDirection::UP:
            up = item;
            break;
        case NavigationDirection::RIGHT:
            right = item;
            break;
        case NavigationDirection::DOWN:
            down = item;
            break;
        case NavigationDirection::NONE:
            break;
        }
    }

    void Navigation::clearDirection(const NavigationDirection direction)
    {
        switch (direction) {
        case (NavigationDirection::LEFT):
            left = nullptr;
            break;
        case (NavigationDirection::UP):
            up = nullptr;
            break;
        case (NavigationDirection::RIGHT):
            right = nullptr;
            break;
        case (NavigationDirection::DOWN):
            down = nullptr;
            break;
        case NavigationDirection::NONE:
            break;
        }
    }

    Item *Navigation::getDirectionItem(const NavigationDirection direction) const
    {
        switch (direction) {
        case (NavigationDirection::LEFT):
            return left;
        case (NavigationDirection::UP):
            return up;
        case (NavigationDirection::RIGHT):
            return right;
        case (NavigationDirection::DOWN):
            return down;
        case NavigationDirection::NONE:
            break;
        }
        return nullptr;
    }

    Navigation::~Navigation()
    {
        left = up = right = down = nullptr;
    }

} /* namespace gui */
