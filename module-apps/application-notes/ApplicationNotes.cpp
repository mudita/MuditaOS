// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationNotes.hpp"

#include "MessageType.hpp"
#include "windows/NotesMainWindow.hpp"
#include "windows/NotesEditWindow.hpp"

#include <service-db/DBMessage.hpp>
#include <service-db/QueryMessage.hpp>

#include <module-apps/application-notes/presenter/NotesMainWindowPresenter.hpp>

namespace app
{
    namespace
    {
        constexpr auto NotesStackSize = 4096U;
    } // namespace

    ApplicationNotes::ApplicationNotes(std::string name, std::string parent, StartInBackground startInBackground)
        : Application(name, parent, startInBackground, NotesStackSize)
    {}

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationNotes::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (static_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        if (resp != nullptr) {
            switch (resp->responseTo) {
            case MessageType::DBQuery:
                if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp); queryResponse != nullptr) {
                    if (auto result = queryResponse->getResult(); result->hasListener()) {
                        if (result->handle()) {
                            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                        }
                    }
                }
                break;
            default:
                break;
            }
            return msgHandled();
        }
        return msgNotHandled();
    }

    sys::ReturnCodes ApplicationNotes::InitHandler()
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);
        return ret;
    }

    sys::ReturnCodes ApplicationNotes::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ApplicationNotes::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationNotes::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [this](Application *app, const std::string &name) {
            auto notesRepository = std::make_shared<notes::NotesListModel>(this);
            auto presenter       = std::make_unique<notes::NotesMainWindowPresenter>(notesRepository);
            return std::make_unique<notes::NotesMainWindow>(app, std::move(presenter));
        });
    }

    void ApplicationNotes::destroyUserInterface()
    {}
} // namespace app
