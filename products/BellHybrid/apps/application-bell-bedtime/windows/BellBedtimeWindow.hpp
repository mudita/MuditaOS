// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellBedtime.hpp"
#include "presenter/BellBedtimeWindowPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellBedtimeWindow : public AppWindow, public app::bell_bedtime::View
    {
      public:
        BellBedtimeWindow(app::ApplicationCommon *app,
                          std::unique_ptr<app::bell_bedtime::BellBedtimeWindowPresenter> &&windowPresenter,
                          std::string name = window::name::bellBedtime);

        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void rebuild() override;
        void exit() override;

      private:
        SideListView *listView{};
        std::unique_ptr<app::bell_bedtime::BellBedtimeWindowPresenter> presenter;
    };
} /* namespace gui */
