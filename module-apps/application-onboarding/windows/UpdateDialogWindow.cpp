// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UpdateDialogWindow.hpp"

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include <module-apps/application-onboarding/data/OnBoardingSwitchData.hpp>
#include <module-apps/messages/DialogMetadataMessage.hpp>

namespace app::onBoarding
{
    UpdateDialogWindow::UpdateDialogWindow(app::Application *app)
        : gui::Dialog(app, gui::window::name::onBoarding_update)
    {
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::ok));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);
    }

    bool UpdateDialogWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                application->switchWindow(gui::window::name::onBoarding_start_configuration,
                                          gui::ShowMode::GUI_SHOW_INIT,
                                          std::make_unique<OnBoardingSwitchData>());
                return true;
            }
            if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_RF)) {
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
