// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/BellSideListItem.hpp>

#include <string>

namespace gui
{
    class DateSetSpinner;

    class DateSetListItem : public BellSideListItem
    {
      public:
        DateSetSpinner *dateSetSpinner = nullptr;

        DateSetListItem();
    };
} /* namespace gui */
