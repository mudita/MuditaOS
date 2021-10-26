// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingDateAndTimeWindow.hpp"
#include "ConfigurationSuccessfulDialogWindow.hpp"

#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-apps/application-onboarding/data/OnBoardingSwitchData.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <header/IceAction.hpp>
#include <service-appmgr/Controller.hpp>

namespace app::onBoarding
{
    OnBoardingDateAndTimeWindow::OnBoardingDateAndTimeWindow(app::ApplicationCommon *app)
        : DateAndTimeMainWindow(app, gui::window::name::onBoarding_date_and_time)
    {
        changeDateAndTimeWindow = gui::window::name::onBoarding_change_date_and_time;
    }

    OnBoardingDateAndTimeWindow::~OnBoardingDateAndTimeWindow()
    {
        clearOptions();
    }

    void OnBoardingDateAndTimeWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        DateAndTimeMainWindow::onBeforeShow(mode, data);
        header->navigationIndicatorAdd(new gui::header::IceAction(), gui::header::BoxSelection::Left);

        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::save));
        navBar->setText(gui::nav_bar::Side::Left, utils::translate(style::strings::common::Switch));
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(style::strings::common::back));
    }

    bool OnBoardingDateAndTimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LF)) {
            return getFocusItem()->onActivated(nullptr);
        }
        else if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::translate("app_onboarding_title_configuration"),
                                    "success_icon_W_G",
                                    utils::translate("app_onboarding_configuration_successful"),
                                    "",
                                    [=]() -> bool { return true; }});
            application->switchWindow(gui::window::name::onBoarding_configuration_successful,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::move(metaData));
            return true;
        }
        else if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT)) {
            app::manager::Controller::sendAction(application,
                                                 app::manager::actions::EmergencyDial,
                                                 std::make_unique<gui::SwitchData>(),
                                                 app::manager::OnSwitchBehaviour::RunInBackground);
            return true;
        }
        else {
            return DateAndTimeMainWindow::onInput(inputEvent);
        }
    }

    bool OnBoardingDateAndTimeWindow::navBarCallback(gui::Item &item)
    {
        setNavBarText(utils::translate(style::strings::common::save), gui::nav_bar::Side::Center);

        if (item.focus) {
            setNavBarText(utils::translate(style::strings::common::Switch), gui::nav_bar::Side::Left);
        }
        else {
            setNavBarText(utils::translate(style::strings::common::select), gui::nav_bar::Side::Left);
        }
        return true;
    }
} // namespace app::onBoarding
