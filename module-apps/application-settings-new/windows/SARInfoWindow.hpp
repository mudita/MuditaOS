// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"

#include "application-settings-new/presenter/SARInfoWindowPresenter.hpp"

#include <module-gui/gui/widgets/Label.hpp>
#include <module-gui/gui/widgets/Text.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    class SARInfoWindow : public gui::AppWindow, public SARInfoWindowContract::View
    {
      public:
        explicit SARInfoWindow(app::Application *app,
                               std::unique_ptr<SARInfoWindowContract::Presenter> &&windowPresenter);
        ~SARInfoWindow() noexcept override;

      private:
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        std::unique_ptr<SARInfoWindowContract::Presenter> presenter;
        gui::Text *sarInfoText = nullptr;
    };
} // namespace gui
