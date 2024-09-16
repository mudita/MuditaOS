// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ApplicationBellSettings.hpp"
#include "presenter/alarm_settings/SnoozePresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsAlarmSettingsSnoozeWindow : public AppWindow, app::bell_settings::SnoozeWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsAlarmSettingsSnooze";
        BellSettingsAlarmSettingsSnoozeWindow(app::ApplicationCommon *app,
                                              std::unique_ptr<app::bell_settings::SnoozePresenter> presenter);

        void buildInterface() override;
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void onClose(CloseReason reason) override;
        void rebuild() override;
        void exit() override;
        void deepRefresh() override;
        void handleError() override;
        void handleDeletedFile() override;

      private:
        SideListView *listView{};
        std::unique_ptr<app::bell_settings::SnoozePresenter> presenter;
    };

} // namespace gui
