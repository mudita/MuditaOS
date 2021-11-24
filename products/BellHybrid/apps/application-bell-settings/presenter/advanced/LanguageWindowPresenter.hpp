// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/Languages.hpp>

#include <vector>
#include <string>

namespace app::bell_settings
{
    class LanguageWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<LanguageWindowContract::View>
        {
          public:
            virtual std::vector<std::string> getLanguages() const         = 0;
            virtual std::string getSelectedLanguage() const               = 0;
            virtual void setLanguage(const std::string &selectedLanguage) = 0;
        };
    };

    class LanguageWindowPresenter : public LanguageWindowContract::Presenter
    {
      private:
        common::Languages languages;
        app::ApplicationCommon *app{};

      public:
        explicit LanguageWindowPresenter(app::ApplicationCommon *app);

        std::vector<std::string> getLanguages() const override;
        std::string getSelectedLanguage() const override;
        void setLanguage(const std::string &selectedLanguage) override;
    };
} // namespace app::bell_settings
