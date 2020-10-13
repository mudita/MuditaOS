#include "ApplicationMeditation.hpp"

#include "windows/MeditationWindow.hpp"
#include "windows/MeditationListViewWindows.hpp"
#include "windows/MeditationTimerWindow.hpp"
#include "windows/Names.hpp"

namespace app
{
    ApplicationMeditation::ApplicationMeditation(std::string name, std::string parent, bool startBackground)
        : Application{name, parent, startBackground}
    {
        state = std::make_unique<gui::OptionsData>();
    }

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
        -> sys::Message_t
    {
        return Application::DataReceivedHandler(msgl);
    }

    auto ApplicationMeditation::SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationMeditation::createUserInterface()
    {
        windows.insert({app::window::name::meditation_main_window, new gui::MeditationWindow(this)});
        windows.insert({app::window::name::meditation_timer, new gui::MeditationTimerWindow(this)});
        windows.insert({app::window::name::meditation_options, new gui::MeditationOptionsWindow(this)});
        windows.insert({app::window::name::meditation_preparation, new gui::PreparationTimeWindow(this)});
    }

    void ApplicationMeditation::destroyUserInterface()
    {}
} // namespace app
