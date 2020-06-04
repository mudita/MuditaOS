#include "ApplicationMessages.hpp"

#include "application-messages/data/SMSTextToSearch.hpp"
#include "windows/MessagesMainWindow.hpp"
#include "windows/NewMessage.hpp"
#include "windows/OptionsMessages.hpp"
#include "windows/OptionsWindow.hpp"
#include "windows/ThreadViewWindow.hpp"
#include "windows/SearchStart.hpp"
#include "windows/SMSTemplatesWindow.hpp"
#include "windows/SearchResults.hpp"

#include <MessageType.hpp>
#include <Dialog.hpp>
#include <i18/i18.hpp>
#include <../module-services/service-db/messages/DBNotificationMessage.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <cassert>
#include <time/time_conversion.hpp>

namespace app
{

    ApplicationMessages::ApplicationMessages(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, 4096 * 2)
    {
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
    }

    ApplicationMessages::~ApplicationMessages()
    {}

    // Invoked upon receiving data message
    sys::Message_t ApplicationMessages::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        if (msgl->messageType == MessageType::DBServiceNotification) {
            auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
            LOG_DEBUG("Received multicast");
            if (msg != nullptr) {
                if ((msg->interface == db::Interface::Name::SMS) ||
                    (msg->interface == db::Interface::Name::SMSThread)) {
                    this->windows[gui::name::window::thread_view]->rebuild();
                    this->windows[gui::name::window::main_window]->rebuild();
                    if (this->getCurrentWindow() == this->windows[gui::name::window::main_window] ||
                        this->getCurrentWindow() == this->windows[gui::name::window::thread_view]) {
                        refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                    }
                    return std::make_shared<sys::ResponseMessage>();
                }
            }
        }
        // this variable defines whether message was processed.
        bool handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBThreadGetLimitOffset:
            case MessageType::DBQuery:
            case MessageType::DBSMSTemplateGetLimitOffset: {
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
    sys::ReturnCodes ApplicationMessages::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow(gui::name::window::main_window);

        return ret;
    }

    sys::ReturnCodes ApplicationMessages::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationMessages::createUserInterface()
    {
        windowOptions = gui::newOptionWindow(this);

        windows.insert({gui::name::window::main_window, new gui::MessagesMainWindow(this)});
        windows.insert({gui::name::window::thread_view, new gui::ThreadViewWindow(this)});
        windows.insert({gui::name::window::new_sms, new gui::NewSMS_Window(this)});
        windows.insert({windowOptions->getName(), windowOptions});
        windows.insert(
            {gui::name::window::dialog, new gui::Dialog(this, gui::name::window::dialog, gui::Dialog::Meta())});
        windows.insert(
            {gui::name::window::dialog_confirm, new gui::DialogConfirm(this, gui::name::window::dialog_confirm)});
        windows.insert(
            {gui::name::window::dialog_yes_no, new gui::DialogYesNo(this, gui::name::window::dialog_yes_no)});
        windows.insert({gui::name::window::thread_sms_search, new gui::SMSSearch(this)});
        windows.insert({gui::name::window::sms_templates, new gui::SMSTemplatesWindow(this)});
        windows.insert({gui::name::window::search_results, new gui::SearchResults(this)});
    }

    void ApplicationMessages::destroyUserInterface()
    {}

    bool ApplicationMessages::removeSMS_thread(const ThreadRecord *record)
    {
        if (record == nullptr) {
            LOG_ERROR("removing null SMS thread!");
            return false;
        }
        else {
            LOG_DEBUG("Removing thread: %" PRIu32, record->dbID);
            auto dialog = dynamic_cast<gui::DialogYesNo *>(windows[gui::name::window::dialog_yes_no]);
            assert(dialog != nullptr);
            auto meta   = dialog->meta;
            meta.action = [=]() -> bool {
                if (!DBServiceAPI::ThreadRemove(this, record->dbID)) {
                    LOG_ERROR("ThreadRemove id=%" PRIu32 " failed", record->dbID);
                    return false;
                }
                this->switchWindow(gui::name::window::main_window);
                return true;
            };
            meta.text       = utils::localize.get("app_messages_thread_delete_confirmation");
            auto contactRec = DBServiceAPI::ContactGetByID(this, record->contactID);
            auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
            meta.title      = cont.getFormattedName();
            meta.icon       = "phonebook_contact_delete_trashcan";
            dialog->update(meta);
            switchWindow(dialog->getName());
            return true;
        }
    }

    bool ApplicationMessages::removeSMS(const SMSRecord &record)
    {
        LOG_DEBUG("Removing sms: %" PRIu32, record.ID);
        auto dialog = dynamic_cast<gui::DialogYesNo *>(windows[gui::name::window::dialog_yes_no]);
        assert(dialog != nullptr);

        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            if (!DBServiceAPI::SMSRemove(this, record.ID)) {
                LOG_ERROR("sSMSRemove id=%" PRIu32 " failed", record.ID);
                return false;
            }
            this->switchWindow(gui::name::window::thread_view);
            return true;
        };
        meta.text  = utils::localize.get("app_messages_message_delete_confirmation");
        meta.title = record.body;
        meta.icon  = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        switchWindow(dialog->getName());
        return true;
    }

    bool ApplicationMessages::searchEmpty(const std::string &query)
    {
        auto dialog = dynamic_cast<gui::Dialog *>(windows[gui::name::window::dialog]);
        assert(dialog);
        auto meta  = dialog->meta;
        meta.icon  = "search_big";
        meta.text  = utils::localize.get("app_messages_thread_no_result");
        meta.title = utils::localize.get("common_results_prefix") + query;
        dialog->update(meta);
        auto data                        = std::make_unique<gui::SwitchData>();
        data->ignoreCurrentWindowOnStack = true;
        switchWindow(dialog->getName(), std::move(data));
        return true;
    }

    bool ApplicationMessages::showSearchResults(const UTF8 &title, const UTF8 &search_text)
    {
        auto name = gui::name::window::search_results;
        windows[name]->setTitle(title);
        switchWindow(name, std::make_unique<SMSTextToSearch>(search_text));
        return true;
    }

    bool ApplicationMessages::showNotification(std::function<bool()> action, bool ignoreCurrentWindowOnStack)
    {
        auto dialog = dynamic_cast<gui::DialogConfirm *>(windows[gui::name::window::dialog_confirm]);
        assert(dialog);
        auto meta   = dialog->meta;
        meta.icon   = "info_big_circle_W_G";
        meta.text   = utils::localize.get("app_messages_no_sim");
        meta.action = action;
        dialog->update(meta);
        auto switchData                        = std::make_unique<gui::SwitchData>();
        switchData->ignoreCurrentWindowOnStack = ignoreCurrentWindowOnStack;
        switchWindow(dialog->getName(), std::move(switchData));
        return true;
    }

    bool ApplicationMessages::sendSms(const UTF8 &number, const UTF8 &body)
    {
        if (number.length() == 0 || body.length() == 0) {
            LOG_WARN("Number or sms body is empty");
            return false;
        }
        SMSRecord record;
        record.number = number;
        record.body   = body;
        record.type   = SMSType::QUEUED;
        auto time     = utils::time::Timestamp();
        record.date   = time.getTime();
        return DBServiceAPI::SMSAdd(this, record) != DB_ID_NONE;
    }
    bool ApplicationMessages::resendSms(const SMSRecord &record)
    {
        auto resendRecord = record;
        resendRecord.type = SMSType::QUEUED;
        // update date sent - it will display an old, failed sms at the the bottom, but this is correct
        auto time         = utils::time::Timestamp();
        resendRecord.date = time.getTime();
        return DBServiceAPI::SMSUpdate(this, resendRecord) != DB_ID_NONE;
    }

    bool ApplicationMessages::handleSendSmsFromThread(const UTF8 &number, const UTF8 &body)
    {
        if (!sendSms(number, body)) {
            return false;
        }

        if (!Store::GSM::get()->simCardInserted()) {
            auto action = [=]() -> bool {
                returnToPreviousWindow();
                return true;
            };
            return showNotification(action);
        }

        return true;
    }

    bool ApplicationMessages::newMessageOptions(const std::string &requestingWindow, gui::Text *text)
    {
        LOG_INFO("New message options");
        if (windowOptions != nullptr) {
            windowOptions->clearOptions();
            windowOptions->addOptions(newMessageWindowOptions(this, requestingWindow, text));
            switchWindow(windowOptions->getName(), nullptr);
        }
        return true;
    }

} /* namespace app */
