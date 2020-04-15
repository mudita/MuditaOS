#include "ApplicationMessages.hpp"

#include "windows/MessagesMainWindow.hpp"
#include "windows/NewMessage.hpp"
#include "windows/OptionsMessages.hpp"
#include "windows/OptionsWindow.hpp"
#include "windows/ThreadViewWindow.hpp"

#include <MessageType.hpp>
#include <Dialog.hpp>
#include <i18/i18.hpp>
#include <../module-services/service-db/messages/DBNotificationMessage.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <cassert>

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
            DBNotificationMessage *msg = dynamic_cast<DBNotificationMessage *>(msgl);
            LOG_DEBUG("Received multicast");
            if ((msg != nullptr) && (msg->baseType == DB::BaseType::SmsDB) &&
                ((msg->notificationType == DB::NotificationType::Updated) ||
                 (msg->notificationType == DB::NotificationType::Added))) {
                if (this->getCurrentWindow() == this->windows[gui::name::window::thread_view]) {
                    LOG_DEBUG("TODO");
                    this->getCurrentWindow()->rebuild();
                }
                return std::make_shared<sys::ResponseMessage>();
            }
        }
        // this variable defines whether message was processed.
        bool handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBThreadGetLimitOffset: {
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
        windows.insert({gui::name::window::thread_rm_confirm,
                        new gui::Dialog(this,
                                        gui::name::window::thread_rm_confirm,
                                        {
                                            .title  = "",
                                            .icon   = "phonebook_contact_delete_trashcan",
                                            .text   = "",
                                            .action = []() -> bool {
                                                LOG_INFO("!");
                                                return true;
                                            },
                                        })});
        windows.insert({gui::name::window::thread_search_none,
                        new gui::Dialog(this,
                                        gui::name::window::thread_search_none,
                                        {.icon = "search_big", .have_choice = false})});
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
            auto dialog = dynamic_cast<gui::Dialog *>(windows[gui::name::window::thread_rm_confirm]);
            if (dialog != nullptr) {
                auto meta   = dialog->meta;
                meta.action = [=]() -> bool {
                    if (!DBServiceAPI::ThreadRemove(this, record->dbID)) {
                        LOG_ERROR("ThreadRemove id=%" PRIu32 " failed", record->dbID);
                        return false;
                    }
                    return this->switchWindow(gui::name::window::main_window);
                };
                meta.text       = utils::localize.get("app_messages_thread_delete_confirmation");
                auto contactRec = DBServiceAPI::ContactGetByID(this, record->contactID);
                auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
                meta.title      = cont.getFormattedName();
                dialog->update(meta);
                return switchWindow(gui::name::window::thread_rm_confirm, nullptr);
            }
            else {
                LOG_ERROR("Dialog bad type!");
                return false;
            }
        }
    }

    bool ApplicationMessages::searchEmpty(const std::string &query)
    {
        auto dialog = dynamic_cast<gui::Dialog *>(windows[gui::name::window::thread_search_none]);
        assert(dialog);
        auto meta  = dialog->meta;
        meta.text  = utils::localize.get("app_messages_thread_no_result");
        meta.title = utils::localize.get("common_results_prefix") + query;
        dialog->update(meta);
        return switchWindow(gui::name::window::thread_search_none, nullptr);
    }

} /* namespace app */
