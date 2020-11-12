// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMeditation.hpp"

#include "windows/MeditationWindow.hpp"
#include "windows/MeditationListViewWindows.hpp"
#include "windows/MeditationTimerWindow.hpp"
#include "windows/Names.hpp"

namespace app
{
    ApplicationMeditation::ApplicationMeditation(std::string name,
                                                 std::string parent,
                                                 StartInBackground startInBackground)
        : Application{name, parent, startInBackground}, state{std::make_unique<gui::OptionsData>()}
    {}

    auto ApplicationMeditation::InitHandler() -> sys::ReturnCodes
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);
        return ret;
    }

    auto ApplicationMeditation::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
        -> sys::MessagePointer
    {
        return Application::DataReceivedHandler(msgl);
    }

    auto ApplicationMeditation::SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationMeditation::createUserInterface()
    {
        windowsFactory.attach(app::window::name::meditation_main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MeditationWindow>(app);
        });
        windowsFactory.attach(app::window::name::meditation_timer, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MeditationTimerWindow>(app);
        });
        windowsFactory.attach(app::window::name::meditation_options, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MeditationOptionsWindow>(app);
        });
        windowsFactory.attach(app::window::name::meditation_preparation, [](Application *app, const std::string &name) {
            return std::make_unique<gui::PreparationTimeWindow>(app);
        });
    }

    void ApplicationMeditation::destroyUserInterface()
    {}
} // namespace app
