// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingLanguageWindow.hpp"
#include <common/options/OptionBellMenu.hpp>

namespace gui
{
    OnBoardingLanguageWindow::OnBoardingLanguageWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::OnBoarding::OnBoardingLanguageWindowPresenter::Presenter> &&presenter)
        : BellOptionWindow(app, gui::name::window::main_window), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        refreshOptions(languageOptionsList(), this->presenter->getSelectedLanguageIndex());
    }

    std::list<Option> OnBoardingLanguageWindow::languageOptionsList()
    {
        std::list<gui::Option> languageOptionList;

        for (auto language : presenter->getLanguages()) {
            languageOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                language,
                [=](gui::Item &item) {
                    presenter->setLanguage(language);
                    return true;
                },
                nullptr,
                this));
        }

        return languageOptionList;
    }
} // namespace gui
