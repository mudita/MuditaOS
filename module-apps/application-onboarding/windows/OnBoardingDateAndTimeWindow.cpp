// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingDateAndTimeWindow.hpp"
#include "ConfigurationSuccessfulDialogWindow.hpp"

#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-apps/application-onboarding/data/OnBoardingSwitchData.hpp>
#include <module-apps/messages/DialogMetadataMessage.hpp>
#include <module-apps/application-settings-new/data/ChangePasscodeData.hpp>

namespace app::onBoarding
{
    OnBoardingDateAndTimeWindow::OnBoardingDateAndTimeWindow(app::Application *app) : DateAndTimeMainWindow(app)
    {}

    OnBoardingDateAndTimeWindow::~OnBoardingDateAndTimeWindow()
    {
        clearOptions();
    }

    void OnBoardingDateAndTimeWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        DateAndTimeMainWindow::onBeforeShow(mode, data);

        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(style::strings::common::Switch));
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
    }

    bool OnBoardingDateAndTimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_RF)) {
            application->switchWindow(
                gui::window::name::onBoarding_configure_passcode,
                gui::ShowMode::GUI_SHOW_INIT,
                std::make_unique<ChangePasscodeData>(ChangePasscodeAction::OnlyProvideNewPasscode));
            return true;
        }
        else if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_LF)) {
            return getFocusItem()->onActivated(nullptr);
        }
        else if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::localize.get("app_onboarding_title_configuration"),
                                    "success_icon_W_G",
                                    utils::localize.get("app_onboarding_configuration_successful"),
                                    "",
                                    [=]() -> bool { return true; }});
            application->switchWindow(gui::window::name::onBoarding_configuration_successful,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::move(metaData));
            return true;
        }
        else {
            return DateAndTimeMainWindow::onInput(inputEvent);
        }
    }

    bool OnBoardingDateAndTimeWindow::bottomBarCallback(gui::Item &item)
    {
        setBottomBarText(utils::localize.get(style::strings::common::save), gui::BottomBar::Side::CENTER);

        if (item.focus) {
            setBottomBarText(utils::localize.get(style::strings::common::Switch), gui::BottomBar::Side::LEFT);
        }
        else {
            setBottomBarText(utils::localize.get(style::strings::common::select), gui::BottomBar::Side::LEFT);
        }
        return true;
    }
} // namespace app::onBoarding
