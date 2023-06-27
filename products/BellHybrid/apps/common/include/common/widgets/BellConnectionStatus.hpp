// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Units.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>

namespace gui
{

    class BellConnectionStatus : public gui::HBox
    {
      public:
        BellConnectionStatus(Item *parent);
        void setFont(const UTF8 &fontName);
        void checkIfConnected(const Store::Battery::State &state);
        void setConnected();

      private:
        Text *statusText = nullptr;
    };
} // namespace gui
