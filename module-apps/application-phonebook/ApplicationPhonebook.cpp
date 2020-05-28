#include "Application.hpp"

#include "ApplicationPhonebook.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "MessageType.hpp"
#include "windows/PhonebookContact.hpp"
#include "windows/PhonebookDialogs.hpp"
#include "windows/PhonebookErrors.hpp"
#include "windows/PhonebookMainWindow.hpp"
#include "windows/PhonebookNewContact.hpp"
#include "windows/PhonebookOptions.hpp"
#include "windows/PhonebookOptionsNamecard.hpp"
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

        gui::AppWindow *window = nullptr;

        window = new PhonebookMainWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new PhonebookNewContact(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new PhonebookContact(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new PhonebookDeleteContact(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new PhonebookBlockContact(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new PhonebookDuplicateSpeedDial(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new DuplicatedContactDialogWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new PhonebookSearch(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new ErrorWindows::NoResults(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new ErrorWindows::ContactBlocked(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new ErrorWindows::ContactBlocked(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new gui::PhonebookSearchResults(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        // window = new PhonebookOptions(this);
        // windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        windows.insert({windowOptions->getName(), windowOptions});

        windows.insert({"Block dialog",
                        new gui::Dialog(this,
                                        "Block dialog",
                                        {
                                            .title  = "",
                                            .icon   = "phonebook_contact_delete_trashcan",
                                            .text   = "",
                                            .action = []() -> bool {
                                              LOG_INFO("!");
                                              return true;
                                            },
                                        })});
        windows.insert({"Dialog",
                        new gui::Dialog(this,
                                        "Dialog",
                                        {
                                            .title  = "",
                                            .icon   = "phonebook_contact_delete_trashcan",
                                            .text   = "",
                                            .action = []() -> bool {
                                                LOG_INFO("!");
                                                return true;
                                            },
                                        })});

        window = new PhonebookOptionsNamecard(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
    }

    void ApplicationPhonebook::destroyUserInterface()
    {}

    bool ApplicationPhonebook::blockContact(const uint32_t contactId)
    {
        LOG_DEBUG("Blocking contact: %" PRIu32, contactId);
        auto dialog = dynamic_cast<gui::Dialog *>(windows["Block dialog"]);
        if (dialog != nullptr) {
            auto meta   = dialog->meta;
            meta.action = [=]() -> bool {
              if (!DBServiceAPI::ContactBlock(this, contactId)) {
                  LOG_ERROR("Contact id=%" PRIu32 "  block failed", contactId);
                  return false;
              }
              return this->switchWindow(gui::name::window::main_window);
            };
            meta.text       = utils::localize.get("app_phonebook_options_block_confirm");
            auto contactRec = DBServiceAPI::ContactGetByID(this, contactId);
            auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
            meta.title      = cont.getFormattedName();
            dialog->update(meta);
            return switchWindow("Block dialog", nullptr);
        }
        else {
            LOG_ERROR("Dialog bad type!");
            return false;
        }
    }

    bool ApplicationPhonebook::removeContact(const uint32_t contactId)
    {
        LOG_DEBUG("Removing contact: %" PRIu32, contactId);
        auto dialog = dynamic_cast<gui::Dialog *>(windows["Dialog"]);
        if (dialog != nullptr) {
            auto meta   = dialog->meta;
            meta.action = [=]() -> bool {
                if (!DBServiceAPI::ContactRemove(this, contactId)) {
                    LOG_ERROR("Contact id=%" PRIu32 "  remove failed", contactId);
                    return false;
                }
                return this->switchWindow(gui::name::window::main_window);
            };
            meta.text       = utils::localize.get("app_phonebook_options_delete_confirm");
            auto contactRec = DBServiceAPI::ContactGetByID(this, contactId);
            auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
            meta.title      = cont.getFormattedName();
            dialog->update(meta);
            return switchWindow("Dialog", nullptr);
        }
        else {
            LOG_ERROR("Dialog bad type!");
            return false;
        }
    }

} /* namespace app */
