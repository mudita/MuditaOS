// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/FocusCommon.hpp"
#include "presenter/FocusSettingsPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;
}

namespace app::focus
{
    class SettingsWindow : public gui::AppWindow, public SettingsContract::View
    {
      public:
        SettingsWindow(app::ApplicationCommon *app,
                       std::unique_ptr<SettingsContract::Presenter> presenter,
                       const std::string &name = window::name::settings);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;
        void exit() override;

      private:
        void switchToExitWindow();

        gui::SideListView *sideListView{nullptr};
        std::unique_ptr<SettingsContract::Presenter> presenter;
        bool isSaveNeeded{false};
    };
} // namespace app::focus
