#include "ApplicationCallLog.hpp"
#include "data/CallLogInternals.hpp"
#include "windows/CallLogDetailsWindow.hpp"
#include "windows/CallLogMainWindow.hpp"
#include "windows/CallLogOptionsWindow.hpp"

#include <service-db/api/DBServiceAPI.hpp>
#include <service-db/messages/DBMessage.hpp>
#include <Dialog.hpp>
#include <OptionWindow.hpp>
#include <i18/i18.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <module-db/queries/calllog/QueryCalllogSetAllRead.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>

using namespace calllog;

namespace app
{

    ApplicationCallLog::ApplicationCallLog(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, 4096)
    {}

    ApplicationCallLog::~ApplicationCallLog()
    {
        LOG_INFO("ApplicationCallLog::destroy");
    }

    // Invoked upon receiving data message
    sys::Message_t ApplicationCallLog::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if ((reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        auto handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBCalllogGetLimitOffset: {
                if (getCurrentWindow()->onDatabaseMessage(resp)) {
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                }
                break;
            }
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
    sys::ReturnCodes ApplicationCallLog::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow(calllog::settings::MainWindowStr);

        return ret;
    }

    sys::ReturnCodes ApplicationCallLog::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationCallLog::createUserInterface()
    {

        gui::AppWindow *window = nullptr;

        window = new gui::CallLogMainWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new gui::CallLogDetailsWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        windowOptions = gui::newOptionWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(windowOptions->getName(), windowOptions));

        window = new gui::DialogYesNo(this, calllog::settings::DialogYesNoStr);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
    }

    void ApplicationCallLog::destroyUserInterface()
    {}

    bool ApplicationCallLog::removeCalllogEntry(const CalllogRecord &record)
    {
        LOG_DEBUG("Removing CalllogRecord: %" PRIu32, record.ID);
        auto dialog = dynamic_cast<gui::DialogYesNo *>(windows[calllog::settings::DialogYesNoStr]);
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            if (DBServiceAPI::CalllogRemove(this, record.ID) == false) {
                LOG_ERROR("CalllogRemove id=%" PRIu32 " failed", record.ID);
                return false;
            }
            this->switchWindow(calllog::settings::MainWindowStr);
            return true;
        };
        meta.title = record.name;
        meta.text  = utils::localize.get("app_calllog_delete_call_confirmation");
        meta.icon  = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        switchWindow(dialog->getName());
        return true;
    }

    bool ApplicationCallLog::setAllEntriesRead()
    {
        // clear also notifications
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));

        return DBServiceAPI::GetQuery(
            this, db::Interface::Name::Calllog, std::make_unique<db::query::calllog::SetAllRead>());
    }

} /* namespace app */
