#include "ApplicationPhonebook.hpp"
#include "Dialog.hpp"
#include "messages/QueryMessage.hpp"
#include "messages/DBNotificationMessage.hpp"
#include "models/PhonebookModel.hpp"
#include "windows/PhonebookContactDetails.hpp"
#include "windows/PhonebookContactOptions.hpp"
#include "windows/PhonebookMainWindow.hpp"
#include "windows/PhonebookNewContact.hpp"
#include "windows/PhonebookNamecardOptions.hpp"
#include "windows/PhonebookSearch.hpp"
#include "windows/PhonebookSearchResults.hpp"
#include "windows/PhonebookIceContacts.hpp"
#include <service-appmgr/ApplicationManager.hpp>

namespace app
{

    ApplicationPhonebook::ApplicationPhonebook(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, phonebook_stack_size)
    {
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
    }

    // Invoked upon receiving data message
    auto ApplicationPhonebook::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::Message_t
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
                    for (auto &[name, window] : windows) {
                        window->onDatabaseMessage(msg);
                    }
                }
                // app-wide actions
                // <none>
                return std::make_shared<sys::ResponseMessage>();
            }
        }

        // this variable defines whether message was processed.
        bool handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBQuery: {

                if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp)) {
                    auto result = queryResponse->getResult();

                    if (result->hasListener()) {
                        if (result->handle()) {
                            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                        }
                    }
                }

            } break;
            default:
                break;
            }
        }

        if (handled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
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

    auto ApplicationPhonebook::DeinitHandler() -> sys::ReturnCodes
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationPhonebook::createUserInterface()
    {
        windows.insert({gui::name::window::main_window, new gui::PhonebookMainWindow(this)});
        windows.insert({gui::window::name::new_contact, new gui::PhonebookNewContact(this)});
        windows.insert({gui::window::name::contact, new gui::PhonebookContactDetails(this)});
        windows.insert({gui::window::name::search, new gui::PhonebookSearch(this)});
        windows.insert({gui::window::name::search_results, new gui::PhonebookSearchResults(this)});
        windows.insert(
            {gui::window::name::dialog, new gui::Dialog(this, gui::window::name::dialog, gui::Dialog::Meta())});
        windows.insert(
            {gui::window::name::dialog_yes_no, new gui::DialogYesNo(this, gui::window::name::dialog_yes_no)});
        windows.insert({gui::window::name::dialog_yes_no_icon_txt,
                        new gui::DialogYesNoIconTxt(this, gui::window::name::dialog_yes_no_icon_txt)});
        windows.insert(
            {gui::window::name::dialog_confirm, new gui::DialogConfirm(this, gui::window::name::dialog_confirm)});
        windows.insert({gui::window::name::contact_options, new gui::PhonebookContactOptions(this)});
        windows.insert({gui::window::name::namecard_options, new gui::PhonebookNamecardOptions(this)});
        windows.insert({gui::window::name::ice_contacts, new gui::PhonebookIceContacts(this)});
    }

    void ApplicationPhonebook::destroyUserInterface()
    {}

    void ApplicationPhonebook::onSearchRequest(const std::string &searchFilter)
    {
        auto searchModel = std::make_unique<PhonebookModel>(this, searchFilter);

        LOG_DEBUG("Search results count: %d", searchModel->requestRecordsCount());
        if (searchModel->requestRecordsCount() > 0) {
            auto main_window = dynamic_cast<gui::PhonebookMainWindow *>(windows[gui::name::window::main_window]);
            if (main_window == nullptr) {
                LOG_ERROR("Failed to get main window.");
                return;
            }

            if (main_window->isSearchRequested()) {
                searchModel->messagesSelectCallback = [=](gui::PhonebookItem *item) {
                    std::unique_ptr<PhonebookSearchReuqest> data = std::make_unique<PhonebookSearchReuqest>();
                    data->result                                 = item->contact;
                    data->setDescription("PhonebookSearchRequest");
                    return sapm::ApplicationManager::messageSwitchPreviousApplication(
                        this, std::make_unique<sapm::APMSwitchPrevApp>(GetName(), std::move(data)));
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
        auto dialog = dynamic_cast<gui::Dialog *>(windows[gui::window::name::dialog]);
        assert(dialog);
        auto meta  = dialog->meta;
        meta.icon  = "search_big";
        meta.text  = utils::localize.get("app_phonebook_search_no_results");
        meta.title = utils::localize.get("common_results_prefix") + "\"" + query + "\"";
        dialog->update(meta);
        auto data                        = std::make_unique<gui::SwitchData>();
        data->ignoreCurrentWindowOnStack = true;
        LOG_DEBUG("Switching to app_phonebook_search_no_results window.");
        switchWindow(dialog->getName(), std::move(data));
        return true;
    }

} /* namespace app */
