// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "TimeUnitsPresenter.hpp"
#include "TimeUnitsModel.hpp"

#include <apps-common/windows/AppWindow.hpp>
#include <widgets/SideListView.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsTimeUnitsWindow : public AppWindow, public app::bell_settings::TimeUnitsWindowContract::View
    {
      public:
        explicit BellSettingsTimeUnitsWindow(
            app::Application *app,
            std::unique_ptr<app::bell_settings::TimeUnitsWindowContract::Presenter> &&windowPresenter,
            std::string name = gui::window::name::bellSettingsTimeUnits);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        SideListView *sidelistview = nullptr;
        std::unique_ptr<app::bell_settings::TimeUnitsWindowContract::Presenter> presenter;
    };
} /* namespace gui */
