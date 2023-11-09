// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingLanguagesWindow.hpp"
#include <application-onboarding/ApplicationOnBoarding.hpp>

#include <header/IceAction.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <service-appmgr/Controller.hpp>
#include <EventStore.hpp>

namespace app::onBoarding
{
    OnBoardingLanguagesWindow::OnBoardingLanguagesWindow(app::ApplicationCommon *app, const std::string &name)
        : LanguagesWindow(app, name)
    {
        applyToStatusBar([](gui::status_bar::Configuration configuration) {
            configuration.setIndicator(gui::status_bar::Indicator::SimCard, false);
            return configuration;
        });
    }

    void OnBoardingLanguagesWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (Store::GSM::get()->simCardInserted()) {
            header->navigationIndicatorAdd(new gui::header::IceAction(), gui::header::BoxSelection::Left);
        }
        navBar->setActive(gui::nav_bar::Side::Right, false);

        LanguagesWindow::onBeforeShow(mode, data);
    }

    bool OnBoardingLanguagesWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            return true;
        }
        else if (inputEvent.isShortRelease(gui::KeyCode::KEY_LEFT) && Store::GSM::get()->simCardInserted()) {
            app::manager::Controller::sendAction(application,
                                                 app::manager::actions::EmergencyDial,
                                                 std::make_unique<gui::SwitchData>(),
                                                 app::manager::OnSwitchBehaviour::RunInBackground);
            return true;
        }
        else {
            return LanguagesWindow::onInput(inputEvent);
        }
    }
} // namespace app::onBoarding
