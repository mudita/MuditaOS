// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCallLog.hpp"
#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "data/CallLogInternals.hpp"
#include "windows/CallLogDetailsWindow.hpp"
#include "windows/CallLogMainWindow.hpp"
#include "windows/CallLogOptionsWindow.hpp"

#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBMessage.hpp>
#include <Dialog.hpp>
#include <OptionWindow.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <module-db/queries/calllog/QueryCalllogSetAllRead.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>

using namespace calllog;

namespace app
{
    ApplicationCallLog::ApplicationCallLog(std::string name, std::string parent, StartInBackground startInBackground)
        : Application(name, parent, startInBackground, 4096)
    {
        addActionReceiver(manager::actions::ShowCallLog, [this](auto &&data) {
            switchWindow(gui::name::window::main_window, std::move(data));
            return msgHandled();
        });
    }

    ApplicationCallLog::~ApplicationCallLog()
    {
        LOG_INFO("ApplicationCallLog::destroy");
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationCallLog::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
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
        windowsFactory.attach(calllog::settings::MainWindowStr, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CallLogMainWindow>(app);
        });
        windowsFactory.attach(calllog::settings::DetailsWindowStr, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CallLogDetailsWindow>(app);
        });
        windowsFactory.attach(
            utils::localize.get("app_phonebook_options_title"),
            [](Application *app, const std::string &name) { return std::make_unique<gui::OptionWindow>(app, name); });
        windowsFactory.attach(calllog::settings::DialogYesNoStr, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
    }

    void ApplicationCallLog::destroyUserInterface()
    {}

    bool ApplicationCallLog::removeCalllogEntry(const CalllogRecord &record)
    {
        LOG_DEBUG("Removing CalllogRecord: %" PRIu32, record.ID);
        gui::DialogMetadata meta;
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
        switchWindow(calllog::settings::DialogYesNoStr, std::make_unique<gui::DialogMetadataMessage>(meta));
        return true;
    }

    bool ApplicationCallLog::setAllEntriesRead()
    {
        // clear also notifications
        DBServiceAPI::GetQuery(this,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));
        const auto [succeed, _] = DBServiceAPI::GetQuery(
            this, db::Interface::Name::Calllog, std::make_unique<db::query::calllog::SetAllRead>());
        return succeed;
    }

} /* namespace app */
