// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SignalStrengthBase.hpp"

namespace gui
{
    class Image;
}
namespace gui::top_bar
{
    class SignalStrengthBar : public SignalStrengthBase
    {
        Image *img = nullptr;

      protected:
        void update() override;

      public:
        SignalStrengthBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    };
} // namespace gui::top_bar
