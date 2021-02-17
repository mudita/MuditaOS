// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"
#include <common_data/EventStore.hpp>

namespace gui
{
    class SignalStrengthWidgetBase : public HBox
    {
      public:
        SignalStrengthWidgetBase(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        virtual void show(const Store::SignalStrength data, bool shown) = 0;
    };
} // namespace gui
