// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-phonebook/ApplicationPhonebook.hpp>
#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"
#include "models/PhonebookModel.hpp"
#include "windows/PhonebookContactDetails.hpp"
#include "windows/PhonebookContactOptions.hpp"
#include "windows/PhonebookMainWindow.hpp"
#include "windows/PhonebookNewContact.hpp"
#include "windows/PhonebookNamecardOptions.hpp"
#include "windows/PhonebookSearch.hpp"
#include "windows/PhonebookSearchResults.hpp"
#include "windows/PhonebookIceContacts.hpp"
#include <service-appmgr/Controller.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <utility>

namespace app
{
    ApplicationPhonebook::ApplicationPhonebook(std::string name,
                                               std::string parent,
                                               sys::phone_modes::PhoneMode phoneMode,
                                               sys::bluetooth::BluetoothMode bluetoothMode,
                                               StartInBackground startInBackground)
        : Application(
              std::move(name), std::move(parent), phoneMode, bluetoothMode, startInBackground, phonebook_stack_size)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        addActionReceiver(manager::actions::ShowContacts, [this](auto &&data) {
            switchWindow(gui::name::window::main_window, std::move(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::AddContact, [this](auto &&data) {
            switchWindow(gui::window::name::new_contact, std::move(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::EditContact, [this](auto &&data) {
            switchWindow(gui::window::name::new_contact, std::move(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::ShowContactDetails, [this](auto &&data) {
            switchWindow(gui::window::name::contact, std::move(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::ShowEmergencyContacts, [this](auto &&data) {
            switchWindow(gui::window::name::ice_contacts, std::move(data));
            return actionHandled();
        });
    }

    // Invoked upon receiving data message
    auto ApplicationPhonebook::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
        -> sys::MessagePointer
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        if (msgl->messageType == MessageType::DBServiceNotification) {
            auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
            LOG_DEBUG("Received notification");
            if (msg != nullptr) {
                // window-specific actions
                if (msg->interface == db::Interface::Name::Contact) {
                    for (auto &[name, window] : windowsStack) {
                        window->onDatabaseMessage(msg);
                    }
                }
                // app-wide actions
                // <none>
                return std::make_shared<sys::ResponseMessage>();
            }
        }

        return handleAsyncResponse(resp);
    }

    // Invoked during initialization
    auto ApplicationPhonebook::InitHandler() -> sys::ReturnCodes
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return ret;
    }

    void ApplicationPhonebook::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookMainWindow>(app);
        });
        windowsFactory.attach(gui::window::name::contact, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookContactDetails>(app);
        });
        windowsFactory.attach(gui::window::name::search, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookSearch>(app);
        });
        windowsFactory.attach(gui::window::name::search_results, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookSearchResults>(app);
        });
        windowsFactory.attach(gui::window::name::dialog, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
        windowsFactory.attach(gui::window::name::dialog_yes_no, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, gui::window::name::dialog_yes_no);
        });
        windowsFactory.attach(gui::window::name::dialog_yes_no_icon_txt,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::DialogYesNoIconTxt>(app, name);
                              });
        windowsFactory.attach(gui::window::name::dialog_confirm, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, gui::window::name::dialog_confirm);
        });
        windowsFactory.attach(gui::window::name::contact_options, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookContactOptions>(app);
        });
        windowsFactory.attach(gui::window::name::namecard_options, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookNamecardOptions>(app);
        });
        windowsFactory.attach(gui::window::name::ice_contacts, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookIceContacts>(app);
        });

        windowsFactory.attach(gui::window::name::new_contact, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookNewContact>(app);
        });

        attachPopups(
            {gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes, gui::popup::ID::PhoneLock});
    }

    void ApplicationPhonebook::destroyUserInterface()
    {}

    void ApplicationPhonebook::onSearchRequest(const std::string &searchFilter)
    {
        auto searchModel = std::make_unique<PhonebookModel>(this, searchFilter);

        LOG_DEBUG("Search results count: %d", searchModel->requestRecordsCount());
        if (searchModel->requestRecordsCount() > 0) {
            auto main_window =
                dynamic_cast<gui::PhonebookMainWindow *>(windowsStack.get(gui::name::window::main_window));
            if (main_window == nullptr) {
                LOG_ERROR("Failed to get main window.");
                return;
            }

            if (main_window->isSearchRequested()) {
                searchModel->messagesSelectCallback = [=](gui::PhonebookItem *item) {
                    std::unique_ptr<PhonebookSearchRequest> data = std::make_unique<PhonebookSearchRequest>();
                    data->result                                 = item->contact;
                    data->setDescription("PhonebookSearchRequest");
                    return app::manager::Controller::switchBack(
                        this, std::make_unique<app::manager::SwitchBackRequest>(GetName(), std::move(data)));
                };
            }
            LOG_DEBUG("Switching to search results window.");
            auto data = std::make_unique<PhonebookSearchResultsData>(std::move(searchModel));
            switchWindow("SearchResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        }
        else {
            searchEmpty(searchFilter);
        }
    }

    bool ApplicationPhonebook::searchEmpty(const std::string &query)
    {
        gui::DialogMetadata meta;
        meta.icon                        = "search_big";
        meta.text                        = utils::translate("app_phonebook_search_no_results");
        meta.title                       = utils::translate("common_results_prefix") + "\"" + query + "\"";
        auto data                        = std::make_unique<gui::DialogMetadataMessage>(meta);
        data->ignoreCurrentWindowOnStack = true;
        LOG_DEBUG("Switching to app_phonebook_search_no_results window.");
        switchWindow(gui::window::name::dialog, std::move(data));
        return true;
    }

} /* namespace app */
