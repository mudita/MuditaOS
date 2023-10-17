// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingFinalizeWindow.hpp"

#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <common/data/StyleCommon.hpp>

namespace gui
{
    OnBoardingFinalizeWindow::OnBoardingFinalizeWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::OnBoarding::OnBoardingFinalizeWindowContract::Presenter> &&presenter,
        const std::string &name)
        : BellFinishedWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void OnBoardingFinalizeWindow::buildInterface()
    {
        BellFinishedWindow::buildInterface();

        icon->image->set("circle_success_big");
        icon->resizeItems();

        timerCallback = [this](Item &, sys::Timer &) {
            presenter->finalizeOnBoarding();
            return true;
        };
    }

    bool OnBoardingFinalizeWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            presenter->finalizeOnBoarding();
            return true;
        }
        return false;
    }
} // namespace gui
