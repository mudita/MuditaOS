// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SignalStrengthBase.hpp"

namespace gui
{
    class Image;
}

namespace gui::status_bar
{
    class SignalStrengthBar : public SignalStrengthBase
    {
      public:
        SignalStrengthBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        [[nodiscard]] bool update(const Store::SignalStrength &signal,
                                  const Store::Network::Status &status,
                                  const Store::Tethering &tethering) override;

      private:
        Image *img = nullptr;
        Store::RssiBar currentRssiBar        = Store::RssiBar::zero;
        Store::Network::Status currentStatus = Store::Network::Status::NotRegistered;
        Store::Tethering currentTethering    = Store::Tethering::Off;
    };
} // namespace gui::status_bar
