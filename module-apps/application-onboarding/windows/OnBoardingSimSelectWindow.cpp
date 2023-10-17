// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingSimSelectWindow.hpp"

#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <application-onboarding/style/OnBoardingStyle.hpp>
#include <OptionSetting.hpp>

#include <header/IceAction.hpp>
#include <service-appmgr/Controller.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>

namespace app::onBoarding
{
    OnBoardingSimSelectWindow::OnBoardingSimSelectWindow(app::ApplicationCommon *app, std::string name)
        : BaseSettingsWindow(app, std::move(name))
    {
        buildInterface();
    }

    void OnBoardingSimSelectWindow::buildInterface()
    {
        setTitle(utils::translate("app_onboarding_select_sim"));
        header->navigationIndicatorAdd(new gui::header::IceAction(), gui::header::BoxSelection::Left);
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(::style::strings::common::select));
        navBar->setText(gui::nav_bar::Side::Right, utils::translate(::style::strings::common::back));
        navBar->setText(gui::nav_bar::Side::Left, utils::translate(::style::strings::common::skip));

        descriptionText = new gui::Text(this,
                                        style::window::default_left_margin,
                                        style::onboarding::sim_select::description_y,
                                        style::window::default_body_width,
                                        style::onboarding::sim_select::description_h);
        descriptionText->setFont(style::window::font::medium);
        descriptionText->setAlignment(
            gui::Alignment{gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top});
        descriptionText->setEdges(gui::RectangleEdge::Top);
        descriptionText->setPenWidth(style::window::default_border_rect_no_focus);
        descriptionText->setPadding(gui::Padding(0, style::onboarding::sim_select::description_top_padding, 0, 0));
        descriptionText->setRichText(utils::translate("app_onboarding_select_sim_description"));
    }

    auto OnBoardingSimSelectWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;

        options.emplace_back(std::make_unique<gui::option::OptionSettings>(
            "SIM1",
            [=](const gui::Item &item) {
                application->getSimLockSubject().setSim(cellular::api::SimSlot::SIM1);
                application->switchWindow(gui::popup::window::sim_switching_window);
                return true;
            },
            nullptr,
            this,
            gui::option::SettingRightItem::SIM1));

        options.emplace_back(std::make_unique<gui::option::OptionSettings>(
            "SIM2",
            [=](const gui::Item &item) {
                application->getSimLockSubject().setSim(cellular::api::SimSlot::SIM2);
                application->switchWindow(gui::popup::window::sim_switching_window);
                return true;
            },
            nullptr,
            this,
            gui::option::SettingRightItem::SIM2));

        return options;
    }

    bool OnBoardingSimSelectWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_LF)) {
            auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
                utils::translate("app_onboarding_title"),
                "info_128px_W_G",
                utils::translate("app_onboarding_skip_confirm"),
                "",
                [=]() -> bool {
                    auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                        gui::DialogMetadata{utils::translate("app_onboarding_no_sim_selected_title"),
                                            "sim_128px_W_G",
                                            utils::translate("app_onboarding_no_sim_selected_description"),
                                            "",
                                            [=]() -> bool { return true; }});

                    application->switchWindow(gui::window::name::onBoarding_no_sim_selected,
                                              gui::ShowMode::GUI_SHOW_INIT,
                                              std::move(metaData));
                    return true;
                }});

            application->switchWindow(
                gui::window::name::onBoarding_skip, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
            return true;
        }
        else if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT)) {
            app::manager::Controller::sendAction(application,
                                                 app::manager::actions::EmergencyDial,
                                                 std::make_unique<gui::SwitchData>(),
                                                 app::manager::OnSwitchBehaviour::RunInBackground);
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
