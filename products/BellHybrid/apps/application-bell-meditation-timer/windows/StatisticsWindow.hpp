// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Contract.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;
} // namespace gui

namespace app::meditation
{
    using namespace gui;

    class StatisticsWindow : public AppWindow, public app::meditation::contract::View
    {
      public:
        static constexpr auto name = "MeditationStatisticsWindow";

        StatisticsWindow(app::ApplicationCommon *app,
                         std::unique_ptr<app::meditation::contract::StatisticsPresenter> presenter);

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        SideListView *sideListView{nullptr};
        std::unique_ptr<app::meditation::contract::StatisticsPresenter> presenter;
    };
} // namespace app::meditation
