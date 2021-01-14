// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationNotes.hpp"

#include "MessageType.hpp"
#include "windows/NoteMainWindow.hpp"
#include "windows/NotePreviewWindow.hpp"
#include "windows/NoteEditWindow.hpp"
#include "windows/SearchEngineWindow.hpp"
#include "windows/SearchResultsWindow.hpp"

#include <service-db/DBMessage.hpp>
#include <service-db/QueryMessage.hpp>

#include <module-apps/application-notes/presenter/NotesMainWindowPresenter.hpp>
#include <module-apps/application-notes/presenter/NotePreviewWindowPresenter.hpp>
#include <module-apps/application-notes/presenter/NoteEditWindowPresenter.hpp>
#include <module-apps/windows/OptionWindow.hpp>
#include <module-apps/windows/Dialog.hpp>
#include <module-services/service-db/service-db/DBNotificationMessage.hpp>

namespace app
{
    namespace
    {
        constexpr auto NotesStackSize = 4096U;
    } // namespace

    ApplicationNotes::ApplicationNotes(std::string name, std::string parent, StartInBackground startInBackground)
        : Application(name, parent, startInBackground, NotesStackSize)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationNotes::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (static_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        if (msgl->messageType == MessageType::DBServiceNotification) {
            auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
            if (msg != nullptr) {
                // window-specific actions
                if (msg->interface == db::Interface::Name::Notes) {
                    for (auto &[name, window] : windowsStack) {
                        window->onDatabaseMessage(msg);
                    }
                }
                return std::make_shared<sys::ResponseMessage>();
            }
        }

        if (resp != nullptr) {
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
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
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            auto notesRepository = std::make_unique<notes::NotesDBRepository>(app);
            auto notesProvider   = std::make_shared<notes::NotesListModel>(app, std::move(notesRepository));
            auto presenter       = std::make_unique<notes::NotesMainWindowPresenter>(notesProvider);
            return std::make_unique<notes::NoteMainWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::note_preview, [](Application *app, const std::string &name) {
            auto notesRepository = std::make_unique<notes::NotesDBRepository>(app);
            auto presenter       = std::make_unique<notes::NotePreviewWindowPresenter>(std::move(notesRepository));
            return std::make_unique<notes::NotePreviewWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::note_edit, [](Application *app, const std::string &name) {
            auto notesRepository = std::make_unique<notes::NotesDBRepository>(app);
            auto presenter       = std::make_unique<notes::NoteEditWindowPresenter>(std::move(notesRepository));
            return std::make_unique<notes::NoteEditWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::notes_search, [](Application *app, const std::string &name) {
            auto notesRepository = std::make_unique<notes::NotesDBRepository>(app);
            auto presenter       = std::make_unique<notes::SearchEngineWindowPresenter>(std::move(notesRepository));
            return std::make_unique<notes::SearchEngineWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::notes_search_result, [](Application *app, const std::string &name) {
            return std::make_unique<notes::SearchResultsWindow>(app);
        });
        windowsFactory.attach(gui::name::window::note_dialog, [](Application *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
        windowsFactory.attach(gui::name::window::note_confirm_dialog, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
        windowsFactory.attach(
            utils::localize.get("app_phonebook_options_title"),
            [](Application *app, const std::string &name) { return std::make_unique<gui::OptionWindow>(app, name); });
    }

    void ApplicationNotes::destroyUserInterface()
    {}
} // namespace app
