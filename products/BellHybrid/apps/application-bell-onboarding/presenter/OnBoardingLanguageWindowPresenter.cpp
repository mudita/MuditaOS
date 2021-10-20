// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingLanguageWindowPresenter.hpp"
#include <service-appmgr/Controller.hpp>

namespace app::OnBoarding
{
    OnBoardingLanguageWindowPresenter::OnBoardingLanguageWindowPresenter(app::ApplicationCommon *app) : app(app)
    {}

    std::vector<std::string> OnBoardingLanguageWindowPresenter::getLanguages()
    {
        auto languages = loader.getAvailableDisplayLanguages();
        std::sort(languages.begin(), languages.end());
        return languages;
    }

    unsigned OnBoardingLanguageWindowPresenter::getSelectedLanguageIndex()
    {
        unsigned int selectedLanguageIndex = 0;
        auto langList                      = getLanguages();

        for (unsigned int langIndex = 0; langIndex < langList.size(); ++langIndex) {
            if (utils::getDisplayLanguage() == langList[langIndex]) {
                selectedLanguageIndex = langIndex;
                break;
            }
        }
        return selectedLanguageIndex;
    }

    void OnBoardingLanguageWindowPresenter::setLanguage(const std::string &selectedLanguage)
    {
        app::manager::Controller::changeDisplayLanguage(app, selectedLanguage);
    }
} // namespace app::OnBoarding
