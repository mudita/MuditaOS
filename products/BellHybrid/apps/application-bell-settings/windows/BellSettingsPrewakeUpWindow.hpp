// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "PrewakeUpModel.hpp"
#include "PrewakeUpPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsPrewakeUpWindow : public AppWindow, public app::bell_settings::PrewakeUpWindowContract::View
    {
      public:
        explicit BellSettingsPrewakeUpWindow(
            app::Application *app,
            std::unique_ptr<app::bell_settings::PrewakeUpWindowContract::Presenter> &&windowPresenter,
            std::string name = gui::window::name::bellSettingsAlarmSettingsPrewakeUp);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        SideListView *sidelistview = nullptr;
        std::unique_ptr<app::bell_settings::PrewakeUpWindowContract::Presenter> presenter;
    };
} /* namespace gui */
