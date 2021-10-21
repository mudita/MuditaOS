// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCallLog.hpp"
#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "data/CallLogInternals.hpp"
#include "windows/CallLogDetailsWindow.hpp"
#include "windows/CallLogMainWindow.hpp"
#include "windows/CallLogOptionsWindow.hpp"

#include <service-db/DBServiceAPI.hpp>
#include <Dialog.hpp>
#include <OptionWindow.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <MessageType.hpp>
#include <module-db/queries/calllog/QueryCalllogSetAllRead.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <utility>

using namespace calllog;

namespace app
{
    ApplicationCallLog::ApplicationCallLog(std::string name,
                                           std::string parent,
                                           StatusIndicators statusIndicators,
                                           StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, 4096)
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        addActionReceiver(manager::actions::ShowCallLog, [this](auto &&data) {
            switchWindow(gui::name::window::main_window, std::move(data));
            return actionHandled();
        });
    }

    ApplicationCallLog::~ApplicationCallLog()
    {
        LOG_INFO("ApplicationCallLog::destroy");
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationCallLog::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        // if message was handled by application's template there is no need to process further.
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (auto responseMsg = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
            responseMsg != nullptr && responseMsg->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        if (msgl->messageType == MessageType::DBServiceNotification) {
            for (auto &[name, window] : windowsStack.windows) {
                window->onDatabaseMessage(msgl);
            }
            return sys::msgHandled();
        }

        return handleAsyncResponse(resp);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationCallLog::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        setActiveWindow(calllog::settings::MainWindowStr);

        return ret;
    }

    sys::ReturnCodes ApplicationCallLog::DeinitHandler()
    {
        return Application::DeinitHandler();
    }

    void ApplicationCallLog::createUserInterface()
    {
        windowsFactory.attach(calllog::settings::MainWindowStr, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::CallLogMainWindow>(app);
        });
        windowsFactory.attach(calllog::settings::DetailsWindowStr, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::CallLogDetailsWindow>(app);
        });
        windowsFactory.attach(window::name::option_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::OptionWindow>(app, name);
        });
        windowsFactory.attach(calllog::settings::DialogYesNoStr, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationCallLog::destroyUserInterface()
    {}

    bool ApplicationCallLog::removeCalllogEntry(const CalllogRecord &record)
    {
        LOG_DEBUG("Removing CalllogRecord: %" PRIu32, record.ID);
        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{record.name,
                                "phonebook_contact_delete_trashcan",
                                utils::translate("app_calllog_delete_call_confirmation"),
                                "",
                                [=]() -> bool {
                                    if (DBServiceAPI::CalllogRemove(this, record.ID) == false) {
                                        LOG_ERROR("CalllogRemove id=%" PRIu32 " failed", record.ID);
                                        return false;
                                    }
                                    this->switchWindow(calllog::settings::MainWindowStr);
                                    return true;
                                }});
        switchWindow(calllog::settings::DialogYesNoStr, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
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
