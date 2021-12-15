// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <presenter/OnBoardingLanguageWindowPresenter.hpp>
#include <common/options/BellOptionWindow.hpp>
#include <BellOnBoardingNames.hpp>

namespace gui
{
    class OnBoardingLanguageWindow : public BellOptionWindow,
                                     public app::OnBoarding::OnBoardingLanguageWindowContract::View
    {
      public:
        OnBoardingLanguageWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::OnBoarding::OnBoardingLanguageWindowPresenter::Presenter> &&presenter,
            const std::string &name = gui::window::name::onBoardingLanguageWindow);
        std::string getSelectedLanguage();

      private:
        std::unique_ptr<app::OnBoarding::OnBoardingLanguageWindowPresenter::Presenter> presenter;

        std::list<Option> languageOptionsList();

        std::string selectedLang = "";
    };
} // namespace gui
