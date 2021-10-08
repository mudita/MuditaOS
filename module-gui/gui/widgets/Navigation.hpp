// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "../Common.hpp"

namespace gui
{

    class Item;

    // Class holds GUI Items for 4 possible directions for navigating using keyboard.
    class Navigation
    {
      protected:
        Item *left  = nullptr;
        Item *up    = nullptr;
        Item *right = nullptr;
        Item *down  = nullptr;

      public:
        /// Sets pointer to the widget that should receive focus after receiving navigation event.
        void setDirectionItem(NavigationDirection direction, Item *item);
        /// Retrives item from specified durection
        Item *getDirectionItem(const NavigationDirection direction) const;
        /// Clears Item's pointer for specified direction.
        void clearDirection(const NavigationDirection direction);

        virtual ~Navigation();
    };

} // namespace gui
