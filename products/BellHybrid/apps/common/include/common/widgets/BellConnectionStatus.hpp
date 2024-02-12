// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
        bool hideIfPossible(const Store::Battery::State &state);
        void show();
        bool isVisible() const;

      private:
        Text *statusText = nullptr;
    };
} // namespace gui
