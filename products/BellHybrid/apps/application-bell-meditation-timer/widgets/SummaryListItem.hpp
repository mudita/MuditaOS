// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/BellSideListItemWithCallbacks.hpp>

#include <chrono>

namespace gui
{
    class TextFixedSize;
}

namespace app::meditation
{
    class SummaryListItem : public gui::BellSideListItemWithCallbacks
    {
      public:
        SummaryListItem(const std::string &topDescription, std::chrono::minutes total, std::chrono::minutes average);
    };
} // namespace app::meditation
