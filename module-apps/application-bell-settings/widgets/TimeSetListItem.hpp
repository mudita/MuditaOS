// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/BellSideListItem.hpp>

#include <string>

namespace gui
{
    class TimeSetFmtSpinner;

    class TimeSetListItem : public BellSideListItem
    {
      public:
        TimeSetFmtSpinner *timeSetFmtSpinner = nullptr;

        TimeSetListItem(gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description);
    };
} /* namespace gui */
