// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <presenter/advanced/LanguageWindowPresenter.hpp>
#include <ApplicationBellSettings.hpp>

#include <apps-common/widgets/spinners/Spinners.hpp>
#include <widgets/BellBaseLayout.hpp>

#include <AppWindow.hpp>

namespace gui
{
    class BellSettingsLanguageWindow : public AppWindow, public app::bell_settings::LanguageWindowContract::View
    {
      public:
        BellSettingsLanguageWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<app::bell_settings::LanguageWindowPresenter::Presenter> &&presenter,
                                   const std::string &name = gui::window::name::bellSettingsLanguage);

        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        std::unique_ptr<app::bell_settings::LanguageWindowPresenter::Presenter> presenter;

        BellBaseLayout *body{};
        UTF8Spinner *spinner = nullptr;
    };
} // namespace gui
