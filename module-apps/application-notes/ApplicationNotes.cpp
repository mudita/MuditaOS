// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-notes/ApplicationNotes.hpp>
#include <presenter/NoteEditWindowPresenter.hpp>
#include <presenter/NotePreviewWindowPresenter.hpp>
#include <presenter/NotesMainWindowPresenter.hpp>
#include <windows/NoteEditWindow.hpp>
#include <windows/NoteCreateWindow.hpp>
#include <windows/NoteMainWindow.hpp>
#include <windows/NotePreviewWindow.hpp>
#include <windows/SearchEngineWindow.hpp>
#include <windows/SearchResultsWindow.hpp>

#include <apps-common/windows/Dialog.hpp>
#include <apps-common/windows/OptionWindow.hpp>
#include <MessageType.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>

#include <utility>

namespace app
{
    namespace
    {
        constexpr auto NotesStackSize = 4096U;
    } // namespace

    ApplicationNotes::ApplicationNotes(std::string name,
                                       std::string parent,
                                       sys::phone_modes::PhoneMode phoneMode,
                                       sys::bluetooth::BluetoothMode bluetoothMode,
                                       StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), phoneMode, bluetoothMode, startInBackground, NotesStackSize)
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
            if (auto msg = dynamic_cast<db::NotificationMessage *>(msgl); msg != nullptr) {
                // window-specific actions
                if (msg->interface == db::Interface::Name::Notes) {
                    for (auto &[name, window] : windowsStack) {
                        window->onDatabaseMessage(msg);
                    }
                }
                return std::make_shared<sys::ResponseMessage>();
            }
        }

        return handleAsyncResponse(resp);
    }

    sys::ReturnCodes ApplicationNotes::InitHandler()
    {
        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return ret;
    }

    sys::ReturnCodes ApplicationNotes::DeinitHandler()
    {
        return Application::DeinitHandler();
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
        windowsFactory.attach(gui::name::window::note_create, [](Application *app, const std::string &name) {
            auto notesRepository = std::make_unique<notes::NotesDBRepository>(app);
            auto presenter       = std::make_unique<notes::NoteCreateWindowPresenter>(std::move(notesRepository));
            return std::make_unique<notes::NoteCreateWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::notes_search, [](Application *app, const std::string &name) {
            return std::make_unique<notes::SearchEngineWindow>(app,
                                                               std::make_unique<notes::SearchEngineWindowPresenter>());
        });
        windowsFactory.attach(gui::name::window::notes_search_result, [](Application *app, const std::string &) {
            auto notesRepository = std::make_unique<notes::NotesDBRepository>(app);
            auto notesProvider   = std::make_shared<notes::NotesSearchListModel>(app, std::move(notesRepository));
            auto presenter       = std::make_unique<notes::NotesSearchWindowPresenter>(notesProvider);
            return std::make_unique<notes::SearchResultsWindow>(app, std::move(presenter));
        });
        windowsFactory.attach(gui::name::window::note_dialog, [](Application *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
        windowsFactory.attach(gui::name::window::note_confirm_dialog, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
        windowsFactory.attach(
            utils::translate("app_phonebook_options_title"),
            [](Application *app, const std::string &name) { return std::make_unique<gui::OptionWindow>(app, name); });

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationNotes::destroyUserInterface()
    {}
} // namespace app
