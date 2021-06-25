// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <models/TimeUnitsModel.hpp>
#include <application-bell-settings/ApplicationBellSettings.hpp>
#include <apps-common/windows/AppWindow.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsTimeUnitsWindow : public AppWindow
    {
      public:
        explicit BellSettingsTimeUnitsWindow(app::Application *app,
                                             std::string name = gui::window::name::bellSettingsTimeUnits);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        SideListView *sidelistview                          = nullptr;
        std::shared_ptr<gui::TimeUnitsModel> timeUnitsModel = nullptr;
    };
} /* namespace gui */
