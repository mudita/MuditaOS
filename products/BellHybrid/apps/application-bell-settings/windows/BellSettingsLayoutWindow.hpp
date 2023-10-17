// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationBellSettings.hpp>
#include <presenter/LayoutWindowPresenter.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

#include <AppWindow.hpp>

namespace gui
{
    class BellSettingsLayoutWindow : public AppWindow, public app::bell_settings::LayoutWindowContract::View
    {
      public:
        BellSettingsLayoutWindow(app::ApplicationCommon *app,
                                 std::unique_ptr<app::bell_settings::LayoutWindowPresenter::Presenter> &&presenter,
                                 const std::string &name = gui::window::name::bellSettingsLayout);

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        void createArrowsOverlay(unsigned int x, unsigned y, unsigned int w, unsigned int h);

        std::unique_ptr<app::bell_settings::LayoutWindowPresenter::Presenter> presenter;
        Image *arrowLeft{};
        Image *arrowRight{};
        WidgetSpinner *spinner = nullptr;
    };
} // namespace gui
