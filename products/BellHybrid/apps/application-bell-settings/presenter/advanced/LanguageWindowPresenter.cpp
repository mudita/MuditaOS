// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LanguageWindowPresenter.hpp"
#include <service-appmgr/Controller.hpp>

namespace app::bell_settings
{
    LanguageWindowPresenter::LanguageWindowPresenter(app::ApplicationCommon *app) : app(app)
    {}

    std::vector<std::string> LanguageWindowPresenter::getLanguages() const
    {
        auto languages = loader.getAvailableDisplayLanguages();
        std::sort(languages.begin(), languages.end());
        return languages;
    }

    std::string LanguageWindowPresenter::getSelectedLanguage() const
    {
        return utils::getDisplayLanguage();
    }

    void LanguageWindowPresenter::setLanguage(const std::string &selectedLanguage)
    {
        app::manager::Controller::changeDisplayLanguage(app, selectedLanguage);
    }
} // namespace app::bell_settings
