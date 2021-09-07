// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <popups/WindowWithTimer.hpp>
#include <Text.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class SimInfoWindow : public WindowWithTimer
    {
        Icon *infoIcon = nullptr;

      public:
        SimInfoWindow(app::Application *app, const std::string &name);

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */
