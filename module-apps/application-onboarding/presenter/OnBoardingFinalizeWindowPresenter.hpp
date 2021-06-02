// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "functional"
#include "string"
#include "BasePresenter.hpp"

using Function = std::function<void()>;

namespace app::onBoarding
{
    class OnBoardingFinalizeContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<OnBoardingFinalizeContract::View>
        {
          public:
            ~Presenter() noexcept override = default;

            virtual void finalizeOnBoarding() = 0;
        };
    };

    class OnBoardingFinalizeWindowPresenter : public OnBoardingFinalizeContract::Presenter
    {
      public:
        explicit OnBoardingFinalizeWindowPresenter(Function finalizeOnBoarding);

        void finalizeOnBoarding() override;

      private:
        std::function<void()> finalizeOnBoardingApp;
    };
} // namespace app::onBoarding
