// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/BellSideListItem.hpp>

#include <string>
#include "widgets/TimeSetFmtSpinner.hpp"

namespace gui
{
    class TimeSetSpinner;

    class TimeSetListItem : public BellSideListItem
    {
      public:
        TimeSetFmtSpinner *timeSetSpinner                                  = nullptr;
        std::function<utils::time::Locale::TimeFormat(void)> getTimeFormat = nullptr;

        TimeSetListItem(gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description);
    };
} /* namespace gui */
