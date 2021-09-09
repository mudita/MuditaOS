// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/presenter/system/SARInfoWindowPresenter.hpp>

#include <AppWindow.hpp>
#include <Text.hpp>

namespace gui
{
    class SARInfoWindow : public gui::AppWindow, public SARInfoWindowContract::View
    {
      public:
        explicit SARInfoWindow(app::ApplicationCommon *app,
                               std::unique_ptr<SARInfoWindowContract::Presenter> &&windowPresenter);

      private:
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void buildInterface() override;

        std::unique_ptr<SARInfoWindowContract::Presenter> presenter;
        gui::Text *sarInfoText = nullptr;
    };
} // namespace gui
