// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <presenter/OnBoardingFinalizeWindowPresenter.hpp>
#include <common/BellFinishedWindow.hpp>

namespace gui
{
    class OnBoardingFinalizeWindow : public BellFinishedWindow,
                                     public app::OnBoarding::OnBoardingFinalizeWindowContract::View
    {
      public:
        OnBoardingFinalizeWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::OnBoarding::OnBoardingFinalizeWindowContract::Presenter> &&presenter);

      private:
        std::unique_ptr<app::OnBoarding::OnBoardingFinalizeWindowContract::Presenter> presenter;

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

} // namespace gui
