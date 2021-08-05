// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/SideListItem.hpp>

#include <string>

namespace gui
{
    class TimeSetFmtSpinner;

    class TimeSetListItem : public SideListItem
    {
      public:
        TimeSetFmtSpinner *timeSetFmtSpinner = nullptr;

        TimeSetListItem(gui::Length x, gui::Length y, gui::Length w, gui::Length h, std::string description);

      private:
        Margins calculateMargins() const noexcept;
    };
} /* namespace gui */
