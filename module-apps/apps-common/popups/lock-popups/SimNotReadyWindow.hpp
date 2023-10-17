// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Text.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    class SimNotReadyWindow : public AppWindow
    {
        Icon *infoIcon = nullptr;

      public:
        SimNotReadyWindow(app::ApplicationCommon *app, const std::string &name);

        void buildInterface() override;
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */
