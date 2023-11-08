// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SignalStrengthBase.hpp"

namespace gui
{
    class Label;
}

namespace gui::status_bar
{
    class SignalStrengthText : public SignalStrengthBase
    {
      public:
        SignalStrengthText(Item *parent, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h);
        [[nodiscard]] bool update(const Store::SignalStrength &signal,
                                  [[maybe_unused]] const Store::Network::Status &status,
                                  [[maybe_unused]] const Store::Tethering &tethering) override;

      private:
        Label *label = nullptr;
        signed currentRssidBm = 0;
    };
} // namespace gui::status_bar
