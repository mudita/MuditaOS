// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

#include <functional>

using FinalizeFunction = std::function<void()>;

namespace app::OnBoarding
{
    class OnBoardingFinalizeWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<OnBoardingFinalizeWindowContract::View>
        {
          public:
            virtual void finalizeOnBoarding() = 0;
        };
    };

    class OnBoardingFinalizeWindowPresenter : public OnBoardingFinalizeWindowContract::Presenter
    {
      public:
        explicit OnBoardingFinalizeWindowPresenter(FinalizeFunction finalizeOnBoarding);

        void finalizeOnBoarding() override;

      private:
        std::function<void()> finalizeOnBoardingApp;
    };
} // namespace app::OnBoarding
