// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "OnBoardingFinalizeWindowPresenter.hpp"

namespace app::OnBoarding
{
    OnBoardingFinalizeWindowPresenter::OnBoardingFinalizeWindowPresenter(FinalizeFunction finalizeOnBoarding)
        : finalizeOnBoardingApp(std::move(finalizeOnBoarding))
    {}

    void OnBoardingFinalizeWindowPresenter::finalizeOnBoarding()
    {
        finalizeOnBoardingApp();
    }
} // namespace app::OnBoarding
