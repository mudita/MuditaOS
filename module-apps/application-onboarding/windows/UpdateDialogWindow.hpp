// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Dialog.hpp"

#include <application-onboarding/presenter/EULALicenseWindowPresenter.hpp>
#include <application-onboarding/presenter/OnBoardingFinalizeWindowPresenter.hpp>

namespace app::onBoarding
{
    class UpdateDialogWindow : public gui::Dialog, public OnBoardingFinalizeContract::View
    {
      private:
        std::unique_ptr<OnBoardingFinalizeContract::Presenter> presenter;

      public:
        UpdateDialogWindow(app::ApplicationCommon *app,
                           std::unique_ptr<OnBoardingFinalizeContract::Presenter> &&windowPresenter);
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace app::onBoarding
