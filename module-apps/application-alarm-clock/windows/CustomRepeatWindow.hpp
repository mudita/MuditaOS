// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-alarm-clock/presenter/CustomRepeatWindowPresenter.hpp>

#include <Application.hpp>
#include <ListView.hpp>
#include <AppWindow.hpp>

namespace app::alarmClock
{
    class CustomRepeatWindow : public gui::AppWindow, public CustomRepeatWindowContract::View
    {
        gui::ListView *list = nullptr;
        std::unique_ptr<CustomRepeatWindowContract::Presenter> presenter;

      public:
        CustomRepeatWindow(app::ApplicationCommon *app,
                           std::unique_ptr<CustomRepeatWindowContract::Presenter> &&windowPresenter);

        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void onClose(CloseReason reason) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
        void buildInterface() override;
    };

} // namespace app::alarmClock
