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
#include "windows/AppsAndToolsWindow.hpp"
#include "windows/NetworkWindow.hpp"
#include "windows/MessagesWindow.hpp"

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
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(
                app, utils::localize.get("app_settings_title_main"), mainWindowOptionsNew(app));
        });

        windowsFactory.attach(gui::window::name::bluetooth, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BluetoothWindow>(app);
        });

        windowsFactory.attach(gui::window::name::display_and_keypad, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DisplayAndKeypadWindow>(app);
        });
        windowsFactory.attach(gui::window::name::input_language, [](Application *app, const std::string &name) {
            return std::make_unique<gui::InputLanguageWindow>(app);
        });
        windowsFactory.attach(gui::window::name::locked_screen, [](Application *app, const std::string &name) {
            return std::make_unique<gui::LockedScreenWindow>(app);
        });
        windowsFactory.attach(gui::window::name::keypad_light, [](Application *app, const std::string &name) {
            return std::make_unique<gui::KeypadLightWindow>(app);
        });
        windowsFactory.attach(gui::window::name::font_size, [](Application *app, const std::string &name) {
            return std::make_unique<gui::FontSizeWindow>(app);
        });
        windowsFactory.attach(gui::window::name::display_light, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DisplayLightWindow>(app);
        });
        windowsFactory.attach(gui::window::name::apps_and_tools, [](Application *app, const std::string &name) {
            return std::make_unique<gui::AppsAndToolsWindow>(app);
        });
        windowsFactory.attach(gui::window::name::network, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NetworkWindow>(app);
        });
        windowsFactory.attach(gui::window::name::messages, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MessagesWindow>(app);
        });
    }

    void ApplicationSettingsNew::destroyUserInterface()
    {}
} /* namespace app */
