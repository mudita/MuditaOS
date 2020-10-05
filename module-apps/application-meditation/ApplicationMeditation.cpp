#include "ApplicationMeditation.hpp"

#include "windows/MeditationWindow.hpp"

namespace app
{
    ApplicationMeditation::ApplicationMeditation(std::string name, std::string parent, bool startBackground)
        : Application{name, parent, startBackground}
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
        auto meditationWindow = new gui::MeditationWindow(this);
        windows.insert({gui::name::window::main_window, meditationWindow});
    }

    void ApplicationMeditation::destroyUserInterface()
    {}
} // namespace app
