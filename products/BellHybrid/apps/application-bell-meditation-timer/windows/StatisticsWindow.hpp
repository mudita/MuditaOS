// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Contract.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;
}

namespace app::meditation
{
    class StatisticsWindow : public gui::AppWindow, public app::meditation::contract::View
    {
      public:
        static constexpr auto name = "MeditationStatisticsWindow";
        StatisticsWindow(app::ApplicationCommon *app, std::unique_ptr<app::meditation::contract::Presenter> presenter);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        gui::SideListView *sideListView{};
        std::unique_ptr<app::meditation::contract::Presenter> presenter;
    };
} // namespace app::meditation
