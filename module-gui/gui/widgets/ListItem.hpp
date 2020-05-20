#pragma once

#include "Rect.hpp"

namespace gui
{

    class ListItem : public Rect
    {
        /// id is valid if value is greater equal to 0.
        int itemID = -1;

      public:

        ListItem();
        virtual void setID(int id)
        {
            itemID = id;
        };
        virtual int getID()
        {
            return itemID;
        };
    };

} /* namespace gui */
