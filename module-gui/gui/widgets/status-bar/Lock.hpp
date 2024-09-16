// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Image.hpp"
#include "StatusBarWidgetBase.hpp"

namespace gui::status_bar
{
    class Lock : public StatusBarWidgetBase<Image>
    {
      public:
        Lock(Item *parent, uint32_t x, uint32_t y);
    };
} // namespace gui::status_bar
