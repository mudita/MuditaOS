// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include <Label.hpp>
#include <common_data/EventStore.hpp>

namespace gui::top_bar
{
    class NetworkAccessTechnology : public StatusBarWidgetBase<Label>
    {
        Store::Network::AccessTechnology _accessTechnology = Store::Network::AccessTechnology::Unknown;

      public:
        NetworkAccessTechnology(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void update(Store::Network::AccessTechnology accessTechnology);
    };
} // namespace gui::top_bar
