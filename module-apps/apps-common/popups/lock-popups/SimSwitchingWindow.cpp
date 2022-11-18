// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimSwitchingWindow.hpp"

#include <Application.hpp>
#include <EventStore.hpp>

namespace constants
{
    constexpr auto timerName = "SimSwitchingTimer";
    inline constexpr std::chrono::seconds timerStartInterval{3};
    inline constexpr std::chrono::seconds timerRepeatInterval{1};
    inline constexpr std::chrono::seconds maxDurationTime{10};
} // namespace constants

namespace gui
{
    SimSwitchingWindow::SimSwitchingWindow(app::ApplicationCommon *app, const std::string &name)
        : WindowWithTimer(app, name)
    {
        buildInterface();
        auto startTime = std::chrono::steady_clock::now();
        timerCallback  = [this, name, startTime](Item &, sys::Timer &timer) {
            if (application->getCurrentWindow()->getName() == name) {
                auto simState = Store::GSM::get()->sim;

                if (switchedSuccessful) {
                    application->getSimLockSubject().simSwitched();
                    application->returnToPreviousWindow();
                }
                else {
                    switch (simState) {
                    case Store::GSM::SIM::SIM1:
                    case Store::GSM::SIM::SIM2:
                    case Store::GSM::SIM::SIM_NEED_PIN:
                    case Store::GSM::SIM::SIM_NEED_PUK:
                    case Store::GSM::SIM::SIM_LOCKED:
                        switchedSuccessful = true;
                        resetTimer(constants::timerStartInterval);
                        updateInterface();
                        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
                        break;
                    case Store::GSM::SIM::NONE:
                    case Store::GSM::SIM::SIM_FAIL:
                    case Store::GSM::SIM::SIM_UNKNOWN:
                        if (std::chrono::steady_clock::now() - startTime >= constants::maxDurationTime) {
                            LOG_INFO("The SIM switch operation failed. We return to the previous window.");
                            application->getSimLockSubject().simSwitched();
                            application->returnToPreviousWindow();
                        }
                        else {
                            resetTimer(constants::timerRepeatInterval);
                        }
                        break;
                    }
                }
                return true;
            }
            else {
                return false;
            }
        };
        resetTimer(constants::timerStartInterval);
    }

    void SimSwitchingWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(utils::translate("app_settings_network_sim_cards"));
        infoIcon = new gui::Icon(this,
                                 style::window::default_left_margin,
                                 style::window::default_vertical_pos,
                                 style::window::default_body_width,
                                 style::window::default_body_height,
                                 "progress_128px_W_G",
                                 utils::translate("sim_card_change_in_progress"));
        infoIcon->setAlignment(Alignment::Horizontal::Center);
    }

    void SimSwitchingWindow::updateInterface()
    {
        setTitle(utils::translate(switchedSuccessful ? "app_settings_net" : "app_settings_network_sim_cards"));
        infoIcon->text->setRichText(
            utils::translate(switchedSuccessful ? "sim_card_changed_successfully" : "sim_card_change_in_progress"));
        infoIcon->image->set(switchedSuccessful ? "success_128px_W_G" : "progress_128px_W_G");
    }

} // namespace gui
