// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include "BoxLayout.hpp"
#include <common_data/EventStore.hpp>

namespace gui
{
    class SignalStrengthWidgetBase : public StatusBarWidgetBase<HBox>
    {
      protected:
        Store::SignalStrength signalStrength;
        virtual void update() = 0;

      public:
        SignalStrengthWidgetBase(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void update(const Store::SignalStrength &data);
    };
} // namespace gui
