// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include "BoxLayout.hpp"
#include <EventStore.hpp>

namespace gui::status_bar
{
    class SignalStrengthBase : public StatusBarWidgetBase<HBox>
    {
      public:
        SignalStrengthBase(Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h);
        [[nodiscard]] virtual bool update(const Store::SignalStrength &signal,
                                          const Store::Network::Status &status,
                                          const Store::Tethering &tethering) = 0;
    };
} // namespace gui::status_bar
