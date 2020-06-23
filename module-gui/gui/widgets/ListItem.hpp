#pragma once

#include "Rect.hpp"

namespace gui
{

    class ListItem : public Rect
    {

      public:
        bool deleteByList = true;

        ListItem();
    };

} /* namespace gui */
