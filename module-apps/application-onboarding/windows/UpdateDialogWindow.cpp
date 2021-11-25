// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateDialogWindow.hpp"

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <service-appmgr/Controller.hpp>
#include <product/version.hpp>

namespace app::onBoarding
{
    UpdateDialogWindow::UpdateDialogWindow(app::ApplicationCommon *app,
                                           std::unique_ptr<OnBoardingFinalizeContract::Presenter> &&windowPresenter)
        : gui::Dialog(app, gui::window::name::onBoarding_update), presenter(std::move(windowPresenter))
    {
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::ok));
        navBar->setActive(gui::nav_bar::Side::Right, false);
        presenter->attach(this);
    }

    void UpdateDialogWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setTitle(utils::translate("app_onboarding_title_update_info"));
        icon->image->set("progress_128px_W_G");
        icon->text->setRichText(utils::translate("app_onboarding_update_info"), {{"$VERSION", std::string(VERSION)}});
    }

    bool UpdateDialogWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                presenter->finalizeOnBoarding();
                return true;
            }
            if (inputEvent.is(gui::KeyCode::KEY_RF)) {
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
