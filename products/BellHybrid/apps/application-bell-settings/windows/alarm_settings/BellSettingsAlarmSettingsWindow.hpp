// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ApplicationBellSettings.hpp"
#include "presenter/alarm_settings/AlarmSettingsPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsAlarmSettingsWindow : public AppWindow,
                                            public app::bell_settings::AlarmSettingsWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsAlarmSettingsWindow";
        explicit BellSettingsAlarmSettingsWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::bell_settings::AlarmSettingsWindowContract::Presenter> presenter);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void exit() override;
        void deepRefresh() override;
        void handleError() override;
        void handleDeletedFile() override;

      private:
        SideListView *sidelistview{};
        std::unique_ptr<app::bell_settings::AlarmSettingsWindowContract::Presenter> presenter;
    };
} /* namespace gui */
