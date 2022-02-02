// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "presenter/FrontlightPresenter.hpp"
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsFrontlightWindow : public AppWindow, public app::bell_settings::FrontlightWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsFrontlight";
        explicit BellSettingsFrontlightWindow(
            app::ApplicationCommon *app, std::unique_ptr<app::bell_settings::FrontlightPresenter> &&windowPresenter);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void exit() override;

      private:
        SideListView *listView{};
        std::unique_ptr<app::bell_settings::FrontlightPresenter> presenter;
    };
} /* namespace gui */
