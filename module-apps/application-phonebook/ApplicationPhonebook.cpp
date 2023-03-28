﻿// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <application-phonebook/ApplicationPhonebook.hpp>
#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"
#include "models/MultipleNumbersModel.hpp"
#include "windows/PhonebookContactDetails.hpp"
#include "windows/PhonebookContactOptions.hpp"
#include "windows/PhonebookMainWindow.hpp"
#include "windows/PhonebookNewContact.hpp"
#include "windows/PhonebookNamecardOptions.hpp"
#include "windows/PhonebookMultipleNumbersSelect.hpp"
#include "windows/PhonebookSearch.hpp"
#include "windows/PhonebookSearchResults.hpp"
#include "windows/PhonebookIceContacts.hpp"
#include "windows/PhonebookInputOptions.hpp"
#include "service-appmgr/messages/SwitchBackRequest.hpp"
#include "service-appmgr/Controller.hpp"
#include <service-db/QueryMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <utility>

namespace app
{
    ApplicationPhonebook::ApplicationPhonebook(std::string name,
                                               std::string parent,
                                               StatusIndicators statusIndicators,
                                               StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, phonebook_stack_size)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        addActionReceiver(manager::actions::ShowContacts, [this](auto &&data) {
            /// Phonebook's contact entries should present different content when the ApplicationPhonebook is invoked
            /// by external application request
            phonebookModel->customContactActivationCallback = [this](const auto &item) {
                if (item->contact->numbers.size() > 1) {
                    std::unique_ptr<PhonebookMultipleNumbersRequest> data =
                        std::make_unique<PhonebookMultipleNumbersRequest>("PhonebookMultipleNumbersRequest",
                                                                          item->contact);

                    switchWindow(gui::window::name::multiple_numbers_select, std::move(data));
                }
                else {
                    std::unique_ptr<PhonebookSearchRequest> data = std::make_unique<PhonebookSearchRequest>();
                    data->result                                 = item->contact;
                    data->setDescription("PhonebookSearchRequest");
                    return app::manager::Controller::switchBack(
                        this, std::make_unique<app::manager::SwitchBackRequest>(GetName(), std::move(data)));
                }
                return true;
            };
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
                userInterfaceDBNotification(msgl,
                                            [&]([[maybe_unused]] sys::Message *, [[maybe_unused]] const std::string &) {
                                                return msg->interface == db::Interface::Name::Contact;
                                            });
                return std::make_shared<sys::ResponseMessage>();
            }
        }

        return handleAsyncResponse(resp);
    }

    // Invoked during initialization
    auto ApplicationPhonebook::InitHandler() -> sys::ReturnCodes
    {

        const auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        phonebookModel = std::make_shared<PhonebookModel>(this);

        createUserInterface();

        return ret;
    }

    void ApplicationPhonebook::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [&](ApplicationCommon *app, const std::string &name) {
            searchRequestModel = std::make_shared<SearchRequestModel>();
            return std::make_unique<gui::PhonebookMainWindow>(app, searchRequestModel, phonebookModel);
        });
        windowsFactory.attach(gui::window::name::contact, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookContactDetails>(app);
        });
        windowsFactory.attach(
            gui::window::name::multiple_numbers_select, [](ApplicationCommon *app, const std::string &name) {
                auto numbersModel = std::make_shared<MultipleNumbersModel>(app);
                return std::make_unique<gui::PhonebookMultipleNumbersSelect>(app, std::move(numbersModel));
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
        windowsFactory.attach(gui::window::name::input_options, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookInputOptions>(app, name);
        });
        windowsFactory.attach(gui::window::name::new_contact, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::PhonebookNewContact>(app);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::BluetoothAuthenticate,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::SimLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationPhonebook::destroyUserInterface()
    {}

    void ApplicationPhonebook::onSearchRequest(const std::string &searchFilter)
    {
        auto model = std::make_shared<PhonebookModel>(*phonebookModel);
        model->setFilter(searchFilter);
        LOG_DEBUG("Switching to search results window.");
        auto data = std::make_unique<PhonebookSearchResultsData>(model, searchRequestModel);
        switchWindow(gui::window::name::search_results, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
    }

} /* namespace app */
