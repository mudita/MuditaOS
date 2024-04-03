// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/Contract.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;
}

namespace app::clock
{
    class BellRefreshClockWindow : public gui::AppWindow, public app::clock::contract::View
    {
      public:
        static constexpr auto name = "BellRefreshClockWindow";
        BellRefreshClockWindow(app::ApplicationCommon *app, std::unique_ptr<app::clock::contract::Presenter> presenter);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        void switchToWindow();

        gui::SideListView *sideListView{};
        std::unique_ptr<app::clock::contract::Presenter> presenter;
        bool isSaveNeeded{false};
    };
} // namespace app::clock
