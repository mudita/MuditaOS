// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingFinalizeWindowPresenter.hpp"

namespace app::onBoarding
{
    OnBoardingFinalizeWindowPresenter::OnBoardingFinalizeWindowPresenter(Function finalizeOnBoarding)
        : finalizeOnBoardingApp(std::move(finalizeOnBoarding))
    {}

    void OnBoardingFinalizeWindowPresenter::finalizeOnBoarding()
    {
        finalizeOnBoardingApp();
    }

} // namespace app::onBoarding
