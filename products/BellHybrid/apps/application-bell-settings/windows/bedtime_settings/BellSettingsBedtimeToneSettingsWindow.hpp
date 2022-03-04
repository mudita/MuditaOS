// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellSettings.hpp"
#include "presenter/alarm_settings/AlarmSettingsPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsBedtimeToneSettingsWindow : public AppWindow,
                                                  public app::bell_settings::AlarmSettingsWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsBedtimeToneSettingsWindow";
        explicit BellSettingsBedtimeToneSettingsWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::bell_settings::AlarmSettingsWindowContract::Presenter> presenter);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void exit() override;

      private:
        SideListView *sidelistview{};
        std::unique_ptr<app::bell_settings::AlarmSettingsWindowContract::Presenter> presenter;
    };
} /* namespace gui */
