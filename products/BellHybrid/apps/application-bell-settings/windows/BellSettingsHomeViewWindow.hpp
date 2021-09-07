// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-bell-settings/ApplicationBellSettings.hpp>

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class BellSettingsHomeViewWindow : public AppWindow
    {
      public:
        explicit BellSettingsHomeViewWindow(app::Application *app,
                                            std::string name = gui::window::name::bellSettingsHomeView);

        void buildInterface() override;
        void rebuild() override;
    };
} /* namespace gui */
