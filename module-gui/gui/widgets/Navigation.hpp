/*
 * Navigation.hpp
 *
 *  Created on: 15 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_NAVIGATION_HPP_
#define MIDDLEWARES_GUI_WIDGETS_NAVIGATION_HPP_

#include "../Common.hpp"
#include "Item.hpp"

namespace gui
{

    class Item;
    /**
     * Class holds GUI Items for 4 possible directions for navigating using keyboard.
     */
    class Navigation
    {
      protected:
        Item *left;
        Item *up;
        Item *right;
        Item *down;

      public:
        /**
         * Sets pointer to the widget that should receive focus after receiving navigation event.
         */
        void setDirectionItem(NavigationDirection direction, Item *item);
        /**
         * Retrives item from specified durection
         */
        Item *getDirectionItem(const NavigationDirection direction);
        /**
         * Clears Item's pointer for specified direction.
         */
        void clearDirection(const NavigationDirection direction);

        Navigation();
        virtual ~Navigation();
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_NAVIGATION_HPP_ */
