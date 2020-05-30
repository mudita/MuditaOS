#include "ApplicationPhonebook.hpp"
#include "windows/PhonebookContact.hpp"
#include "windows/PhonebookContactOptions.hpp"
#include "windows/PhonebookDialogs.hpp"
#include "windows/PhonebookErrors.hpp"
#include "windows/PhonebookMainWindow.hpp"
#include "windows/PhonebookNewContact.hpp"
#include "windows/PhonebookNamecardOptions.hpp"
#include "windows/PhonebookSearch.hpp"
#include "windows/PhonebookSearchResults.hpp"

namespace app
{

    ApplicationPhonebook::ApplicationPhonebook(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, 8192)
    {}

    ApplicationPhonebook::~ApplicationPhonebook()
    {}

    // Invoked upon receiving data message
    sys::Message_t ApplicationPhonebook::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        // this variable defines whether message was processed.
        bool handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBContactGetLimitOffset: {
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
    sys::ReturnCodes ApplicationPhonebook::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        return ret;
    }

    sys::ReturnCodes ApplicationPhonebook::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationPhonebook::createUserInterface()
    {
        windows.insert({gui::name::window::main_window, new PhonebookMainWindow(this)});
        windows.insert({gui::window::name::new_contact, new PhonebookNewContact(this)});
        windows.insert({gui::window::name::contact, new PhonebookContact(this)});
        windows.insert({gui::window::name::duplicated_contact, new PhonebookDuplicateSpeedDial(this)});
        windows.insert({gui::window::name::search, new PhonebookSearch(this)});
        windows.insert({gui::window::name::no_results, new NoResults(this)});
        windows.insert({gui::window::name::contact_blocked, new ContactBlocked(this)});
        windows.insert({gui::window::name::search_results, new PhonebookSearchResults(this)});
        windows.insert(
            {gui::window::name::dialog_yes_no, new gui::DialogYesNo(this, gui::window::name::dialog_yes_no)});
        windows.insert({gui::window::name::contact_options, new PhonebookContactOptions(this)});
        windows.insert({gui::window::name::namecard_options, new PhonebookNamecardOptions(this)});
    }

    void ApplicationPhonebook::destroyUserInterface()
    {}

} /* namespace app */
