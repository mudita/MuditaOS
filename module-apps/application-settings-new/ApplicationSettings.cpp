#include "Application.hpp"

#include "MessageType.hpp"
#include "windows/BluetoothWindow.hpp"
#include "windows/SettingsMainWindow.hpp"
#include "windows/DisplayAndKeypadWindow.hpp"
#include "windows/InputLanguageWindow.hpp"
#include "windows/LockedScreenWindow.hpp"
#include "windows/FontSizeWindow.hpp"
#include "windows/DisplayLightWindow.hpp"
#include "windows/KeypadLightWindow.hpp"
#include "windows/NetworkWindow.hpp"

#include "ApplicationSettings.hpp"

#include <module-services/service-evtmgr/api/EventManagerServiceAPI.hpp>
#include <module-apps/messages/AppMessage.hpp>

#include <i18/i18.hpp>

namespace app
{
    ApplicationSettingsNew::ApplicationSettingsNew(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound)
    {}

    // Invoked upon receiving data message
    auto ApplicationSettingsNew::DataReceivedHandler(sys::DataMessage *msgl,
                                                     [[maybe_unused]] sys::ResponseMessage *resp) -> sys::Message_t
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        auto rm = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        if (nullptr != rm && sys::ReturnCodes::Success == rm->retCode) {
            return retMsg;
        }

        return std::make_shared<sys::ResponseMessage>();
    }

    // Invoked during initialization
    auto ApplicationSettingsNew::InitHandler() -> sys::ReturnCodes
    {
        board = EventManagerServiceAPI::GetBoard(this);

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    void ApplicationSettingsNew::createUserInterface()
    {

        gui::AppWindow *window = nullptr;

        window = newOptionWindow(this, gui::name::window::main_window, mainWindowOptionsNew(this));
        window->setTitle(utils::localize.get("app_settings_title_main_new"));
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        windows.insert({gui::window::name::bluetooth, new gui::BluetoothWindow(this)});

        attachWindow(new gui::DisplayAndKeypadWindow(this));
        attachWindow(new gui::InputLanguageWindow(this));
        attachWindow(new gui::LockedScreenWindow(this));
        attachWindow(new gui::KeypadLightWindow(this));
        attachWindow(new gui::FontSizeWindow(this));
        attachWindow(new gui::DisplayLightWindow(this));
        attachWindow(new gui::NetworkWindow(this));
    }

    void ApplicationSettingsNew::destroyUserInterface()
    {}
} /* namespace app */
