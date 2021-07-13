// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/SideListItem.hpp>

#include <string>

namespace gui
{
    class TimeSetSpinner;

    class TimeSetSpinnerListItem : public SideListItem
    {
      public:
        TimeSetSpinner *timeSetSpinner = nullptr;

        TimeSetSpinnerListItem(gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description);
    };
} /* namespace gui */
