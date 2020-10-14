/*
 * @file ApplicationNotes.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 30 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "MessageType.hpp"
#include "windows/NotesMainWindow.hpp"
#include "windows/NotesEditWindow.hpp"
#include "service-db/messages/DBMessage.hpp"

#include "ApplicationNotes.hpp"

namespace app
{

    ApplicationNotes::ApplicationNotes(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, 4096)
    {}

    ApplicationNotes::~ApplicationNotes()
    {
        LOG_INFO("deleting");
    }

    // Invoked upon receiving data message
    sys::Message_t ApplicationNotes::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if ((reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        bool handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBNotesGetLimitOffset: {
                if (getCurrentWindow()->onDatabaseMessage(resp))
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            } break;
            default:
                break;
            }
        }

        if (handled)
            return std::make_shared<sys::ResponseMessage>();
        else
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationNotes::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    sys::ReturnCodes ApplicationNotes::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationNotes::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NotesMainWindow>(app);
        });
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NotesEditWindow>(app);
        });
    }

    void ApplicationNotes::destroyUserInterface()
    {}

} /* namespace app */
