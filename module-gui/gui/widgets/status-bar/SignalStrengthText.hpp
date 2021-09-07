// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        SignalStrengthText(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void update(const Store::SignalStrength &signal,
                    [[maybe_unused]] const Store::Network::Status &status) override;

      private:
        Label *label = nullptr;
    };
} // namespace gui::status_bar
