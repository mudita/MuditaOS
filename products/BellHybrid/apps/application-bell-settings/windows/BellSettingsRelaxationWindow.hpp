// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ApplicationBellSettings.hpp"
#include "presenter/RelaxationWindowPresenter.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsRelaxationWindow : public AppWindow, public app::bell_settings::View
    {
      public:
        BellSettingsRelaxationWindow(app::ApplicationCommon *app,
                                     std::unique_ptr<app::bell_settings::RelaxationWindowPresenter> &&windowPresenter,
                                     std::string name = window::name::bellSettingsRelaxation);

        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void rebuild() override;
        void exit() override;

      private:
        SideListView *listView{};
        std::unique_ptr<app::bell_settings::RelaxationWindowPresenter> presenter;
    };
} /* namespace gui */
