// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include "ListItemWithDescription.hpp"

namespace gui
{
    class SideListItem : public ListItemWithDescription
    {
      public:
        HBox *body = nullptr;

        SideListItem(std::string description);
    };
} /* namespace gui */
