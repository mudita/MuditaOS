// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Contract.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;
}

namespace app::meditation
{
    using namespace gui;

    class SettingsWindow : public AppWindow, public app::meditation::contract::View
    {
      public:
        static constexpr auto name = "MeditationSettingsWindow";

        SettingsWindow(app::ApplicationCommon *app, std::unique_ptr<app::meditation::contract::Presenter> presenter);

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        void switchToExitWindow();

        SideListView *sideListView{nullptr};
        std::unique_ptr<app::meditation::contract::Presenter> presenter;
        bool isSaveNeeded{false};
    };
} // namespace app::meditation
