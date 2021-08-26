// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "presenter/FrontlightPresenter.hpp"
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class BellBaseLayout;

    class BellSettingsFrontlightWindow : public AppWindow, public app::bell_settings::FrontlightWindowContract::View
    {
      public:
        explicit BellSettingsFrontlightWindow(
            app::Application *app,
            std::unique_ptr<app::bell_settings::FrontlightWindowContract::Presenter> &&windowPresenter,
            std::string name = window::name::bellSettingsFrontlight);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;

      private:
        BellBaseLayout *body{nullptr};
        Label *topText{nullptr};
        std::unique_ptr<app::bell_settings::FrontlightWindowContract::Presenter> presenter;
    };
} /* namespace gui */
