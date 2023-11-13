﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StartConfigurationWindow.hpp"

#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <application-onboarding/data/OnBoardingSwitchData.hpp>
#include <application-onboarding/style/OnBoardingStyle.hpp>

#include <apps-common/windows/DialogMetadata.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <header/IceAction.hpp>
#include <service-appmgr/Controller.hpp>

#include <i18n/i18n.hpp>
#include <Style.hpp>
#include <InputEvent.hpp>
#include <EventStore.hpp>

namespace app::onBoarding
{
    StartConfigurationWindow::StartConfigurationWindow(app::ApplicationCommon *app)
        : gui::AppWindow(app, gui::window::name::onBoarding_start_configuration)
    {
        buildInterface();
    }

    void StartConfigurationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        if (Store::GSM::get()->simCardInserted()) {
            header->navigationIndicatorAdd(new gui::header::IceAction(), gui::header::BoxSelection::Left);
        }
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(::style::strings::common::start));
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(::style::strings::common::back));
        navBar->setText(gui::nav_bar::Side::Left, utils::translate(::style::strings::common::skip));

        auto icon = new gui::Icon(this,
                                  style::window::default_left_margin,
                                  style::window::default_vertical_pos,
                                  style::window::default_body_width,
                                  style::window::default_body_height,
                                  "logo_no_text",
                                  utils::translate("app_onboarding_start_configuration"));
        icon->image->setMargins({0, style::onboarding::start_configuration::image_top_margin, 0, 0});
        icon->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    }

    gui::status_bar::Configuration StartConfigurationWindow::configureStatusBar(
        gui::status_bar::Configuration appConfiguration)
    {
        appConfiguration.setIndicator(gui::status_bar::Indicator::SimCard, false);
        return appConfiguration;
    }

    bool StartConfigurationWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_RF)) {
                application->switchWindow(gui::window::name::onBoarding_eula,
                                          gui::ShowMode::GUI_SHOW_INIT,
                                          std::make_unique<OnBoardingSwitchData>());
            }
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                application->switchWindow(gui::window::name::onBoarding_sim_select,
                                          gui::ShowMode::GUI_SHOW_INIT,
                                          std::make_unique<OnBoardingSwitchData>());
            }
            if (inputEvent.is(gui::KeyCode::KEY_LEFT) && Store::GSM::get()->simCardInserted()) {
                app::manager::Controller::sendAction(application,
                                                     app::manager::actions::EmergencyDial,
                                                     std::make_unique<gui::SwitchData>(),
                                                     app::manager::OnSwitchBehaviour::RunInBackground);
            }
            if (inputEvent.is(gui::KeyCode::KEY_LF)) {

                auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                    utils::translate("app_onboarding_title"),
                    "info_128px_W_G",
                    utils::translate("app_onboarding_skip_confirm"),
                    "",
                    [=]() -> bool {
                        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                            gui::DialogMetadata{utils::translate("app_onboarding_title_configuration"),
                                                "info_128px_W_G",
                                                utils::translate("app_onboarding_no_configuration"),
                                                "",
                                                [=]() -> bool { return true; }});

                        application->switchWindow(gui::window::name::onBoarding_no_configuration,
                                                  gui::ShowMode::GUI_SHOW_INIT,
                                                  std::move(metaData));
                        return true;
                    }});

                application->switchWindow(
                    gui::window::name::onBoarding_skip, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
            }
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
