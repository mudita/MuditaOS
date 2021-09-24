// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellSettings.hpp"
#include "presenter/alarm_settings/PrewakeUpPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsPrewakeUpWindow : public AppWindow, public app::bell_settings::PrewakeUpWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsAlarmSettingsPrewakeUp";
        explicit BellSettingsPrewakeUpWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::bell_settings::PrewakeUpWindowContract::Presenter> presenter);

        void buildInterface() override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void exit() override;

      private:
        SideListView *sidelistview{};
        std::unique_ptr<app::bell_settings::PrewakeUpWindowContract::Presenter> presenter;
    };
} /* namespace gui */
