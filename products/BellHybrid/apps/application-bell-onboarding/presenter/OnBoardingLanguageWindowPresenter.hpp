// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/ApplicationCommon.hpp>

#include <vector>
#include <string>

namespace app::OnBoarding
{
    class OnBoardingLanguageWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<OnBoardingLanguageWindowContract::View>
        {
          public:
            virtual std::vector<std::string> getLanguages()               = 0;
            virtual unsigned int getSelectedLanguageIndex()               = 0;
            virtual void setLanguage(const std::string &selectedLanguage) = 0;
        };
    };

    class OnBoardingLanguageWindowPresenter : public OnBoardingLanguageWindowContract::Presenter
    {
      private:
        app::ApplicationCommon *app;

      public:
        explicit OnBoardingLanguageWindowPresenter(app::ApplicationCommon *app);

        std::vector<std::string> getLanguages() override;
        unsigned int getSelectedLanguageIndex() override;
        void setLanguage(const std::string &selectedLanguage) override;
    };
} // namespace app::OnBoarding
