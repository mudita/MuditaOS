// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
