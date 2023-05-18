// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Units.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>

namespace gui
{

    namespace connected
    {
        constexpr auto font_small         = style::window::font::largelight;
        constexpr auto image_left_margin  = 8U;
        constexpr auto connected_widget_h   = 64U;
        constexpr auto connected_widget_w   = 184U;
        constexpr auto status_h          = 102U;


    } // namespace battery

    enum class ConnectionStatusMode
    {
        Show, // If device connected to PC
        Hide  // Otherwise
    };

    class BellConnectionStatus : public gui::HBox
    {
      public:
        BellConnectionStatus(Item *parent);
        void setFont(const UTF8 &fontName);
        void update(const Store::Battery::State& state);

      private:

        ConnectionStatusMode connectionStatusMode = ConnectionStatusMode::Hide;
        Text *statusText                     = nullptr;
    };
} // namespace gui
