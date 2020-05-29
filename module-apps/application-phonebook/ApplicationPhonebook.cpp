#include "Application.hpp"

#include "ApplicationPhonebook.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "MessageType.hpp"
#include "windows/PhonebookContact.hpp"
#include "windows/PhonebookDialogs.hpp"
#include "windows/PhonebookErrors.hpp"
#include "windows/PhonebookMainWindow.hpp"
#include "windows/PhonebookNewContact.hpp"
#include "windows/PhonebookNamecardOptions.hpp"
#include "windows/PhonebookContactOptions.hpp"
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
        windowOptions = gui::newOptionWindow(this);

        windows.insert({gui::name::window::main_window, new PhonebookMainWindow(this)});
        windows.insert({gui::window::name::newContact, new PhonebookNewContact(this)});
        windows.insert({gui::window::name::contact, new PhonebookContact(this)});
        windows.insert({gui::window::name::duplicatedContact, new PhonebookDuplicateSpeedDial(this)});
        windows.insert({gui::window::name::duplicatedContact, new DuplicatedContactDialogWindow(this)});
        windows.insert({gui::window::name::search, new PhonebookSearch(this)});
        windows.insert({gui::window::name::noResults, new NoResults(this)});
        windows.insert({gui::window::name::contactBlocked, new ContactBlocked(this)});
        windows.insert({gui::window::name::search_results, new PhonebookSearchResults(this)});
        windows.insert({windowOptions->getName(), windowOptions});
        windows.insert(
            {gui::name::window::dialogYesNo, new gui::DialogYesNo(this, gui::name::window::dialogYesNo)});
        windows.insert({gui::window::name::options_namecard, new PhonebookNamecardOptions(this)});
    }

    void ApplicationPhonebook::destroyUserInterface()
    {}

    bool ApplicationPhonebook::blockContact(const uint32_t contactId)
    {
        LOG_DEBUG("Blocking contact: %" PRIu32, contactId);
        auto dialog = dynamic_cast<gui::DialogYesNo *>(windows[gui::name::window::dialogYesNo]);
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            if (!DBServiceAPI::ContactBlock(this, contactId)) {
                LOG_ERROR("Contact id=%" PRIu32 "  block failed", contactId);
                return false;
            }
            this->switchWindow(gui::name::window::main_window);
            return true;
        };
        meta.text       = utils::localize.get("app_phonebook_options_block_confirm");
        auto contactRec = DBServiceAPI::ContactGetByID(this, contactId);
        auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
        meta.title      = cont.getFormattedName();
        meta.icon       = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        switchWindow(dialog->getName());
        return true;
    }

    bool ApplicationPhonebook::removeContact(const uint32_t contactId)
    {
        LOG_DEBUG("Removing contact: %" PRIu32, contactId);
        auto dialog = dynamic_cast<gui::DialogYesNo *>(windows[gui::name::window::dialogYesNo]);
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            if (!DBServiceAPI::ContactRemove(this, contactId)) {
                LOG_ERROR("Contact id=%" PRIu32 "  remove failed", contactId);
                return false;
            }
            this->switchWindow(gui::name::window::main_window);
            return true;
        };
        meta.text       = utils::localize.get("app_phonebook_options_delete_confirm");
        auto contactRec = DBServiceAPI::ContactGetByID(this, contactId);
        auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
        meta.title      = cont.getFormattedName();
        meta.icon       = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        switchWindow(dialog->getName());
        return true;
    }

} /* namespace app */
