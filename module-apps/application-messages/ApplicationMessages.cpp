// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesMainWindow.hpp"
#include "NewMessage.hpp"
#include "OptionsMessages.hpp"
#include "SearchResults.hpp"
#include "SearchStart.hpp"
#include "SMSTemplatesWindow.hpp"
#include "SMSTextToSearch.hpp"
#include "SMSThreadViewWindow.hpp"

#include <Dialog.hpp>
#include <DialogMetadata.hpp>
#include <DialogMetadataMessage.hpp>
#include <i18n/i18n.hpp>
#include <memory>
#include <messages/OptionsWindow.hpp>
#include <MessageType.hpp>
#include <module-db/queries/messages/sms/QuerySMSAdd.hpp>
#include <module-db/queries/messages/sms/QuerySMSRemove.hpp>
#include <module-db/queries/messages/sms/QuerySMSUpdate.hpp>
#include <module-db/queries/messages/threads/QueryThreadGetByID.hpp>
#include <module-db/queries/messages/threads/QueryThreadRemove.hpp>
#include <module-db/queries/phonebook/QueryContactGetByID.hpp>
#include <OptionsWindow.hpp>
#include <OptionWindow.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/QueryMessage.hpp>

#include <cassert>
#include <ctime>

namespace app
{
    static constexpr auto messagesStackDepth = 1024 * 6; // 6Kb stack size

    ApplicationMessages::ApplicationMessages(std::string name,
                                             std::string parent,
                                             sys::phone_modes::PhoneMode phoneMode,
                                             sys::bluetooth::BluetoothMode bluetoothMode,
                                             StartInBackground startInBackground)
        : Application(name, parent, phoneMode, bluetoothMode, startInBackground, messagesStackDepth),
          AsyncCallbackReceiver{this}
    {
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        addActionReceiver(manager::actions::CreateSms, [this](auto &&data) {
            switchWindow(gui::name::window::new_sms, std::move(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::ShowSmsTemplates, [this](auto &&data) {
            switchWindow(gui::name::window::sms_templates, std::move(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::SmsRejectNoSim, [this](auto &&data) {
            auto action = [this]() {
                returnToPreviousWindow();
                return true;
            };
            const auto notification = utils::translate("app_messages_no_sim");
            showNotification(action, notification);
            return actionHandled();
        });
        addActionReceiver(manager::actions::SMSRejectedByOfflineNotification, [this](auto &&data) {
            auto action = [=]() -> bool {
                returnToPreviousWindow();
                return true;
            };
            const auto notification = utils::translate("app_sms_offline");
            showNotification(action, notification);

            return actionHandled();
        });
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationMessages::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if (reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        if (msgl->messageType == MessageType::DBServiceNotification) {
            auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
            if (msg != nullptr) {
                // window-specific actions
                if (msg->interface == db::Interface::Name::SMSThread || msg->interface == db::Interface::Name::SMS) {
                    for (auto &[name, window] : windowsStack.windows) {
                        window->onDatabaseMessage(msg);
                    }
                }
                // app-wide actions
                // <none>
                return std::make_shared<sys::ResponseMessage>();
            }
        }

        return handleAsyncResponse(resp);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationMessages::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return ret;
    }

    void ApplicationMessages::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::MessagesMainWindow>(app);
        });
        windowsFactory.attach(gui::name::window::thread_view, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SMSThreadViewWindow>(app);
        });
        windowsFactory.attach(gui::name::window::new_sms, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::NewMessageWindow>(app);
        });
        windowsFactory.attach(utils::translate("app_phonebook_options_title"),
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::OptionWindow>(app, name);
                              });
        windowsFactory.attach(gui::name::window::dialog, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
        windowsFactory.attach(gui::name::window::dialog_confirm, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });
        windowsFactory.attach(gui::name::window::dialog_yes_no, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
        windowsFactory.attach(
            gui::name::window::thread_sms_search,
            [](ApplicationCommon *app, const std::string &name) { return std::make_unique<gui::SMSSearch>(app); });
        windowsFactory.attach(gui::name::window::sms_templates, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SMSTemplatesWindow>(app);
        });
        windowsFactory.attach(gui::name::window::search_results, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::SearchResults>(app);
        });

        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::PhoneLock,
                      gui::popup::ID::Alarm});
    }

    void ApplicationMessages::destroyUserInterface()
    {}

    bool ApplicationMessages::markSmsThreadAsRead(const uint32_t id)
    {
        using namespace db::query;
        LOG_DEBUG("markSmsThreadAsRead");
        DBServiceAPI::GetQuery(
            this, db::Interface::Name::SMSThread, std::make_unique<MarkAsRead>(id, MarkAsRead::Read::True));
        return true;
    }

    bool ApplicationMessages::markSmsThreadAsUnread(const uint32_t id)
    {
        using namespace db::query;
        LOG_DEBUG("markSmsThreadAsUnRead");
        DBServiceAPI::GetQuery(
            this, db::Interface::Name::SMSThread, std::make_unique<MarkAsRead>(id, MarkAsRead::Read::False));
        return true;
    }

    bool ApplicationMessages::removeSmsThread(const ThreadRecord *record)
    {
        using db::query::ContactGetByID;
        using db::query::ContactGetByIDResult;

        if (record == nullptr) {
            LOG_ERROR("Trying to remove a null SMS thread!");
            return false;
        }
        LOG_DEBUG("Removing thread: %" PRIu32, record->ID);

        auto query = std::make_unique<ContactGetByID>(record->contactID, true);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
        task->setCallback([this, record](auto response) {
            auto result = dynamic_cast<ContactGetByIDResult *>(response);
            if (result != nullptr) {
                const auto &contact = result->getResult();
                auto metaData       = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{contact.getFormattedName(),
                                        "phonebook_contact_delete_trashcan",
                                        utils::translate("app_messages_thread_delete_confirmation"),
                                        "",
                                        [this, record]() { return onRemoveSmsThreadConfirmed(*record); }});
                switchWindow(gui::name::window::dialog_yes_no, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
                return true;
            }
            return false;
        });
        task->execute(this, this);
        return true;
    }

    bool ApplicationMessages::onRemoveSmsThreadConfirmed(const ThreadRecord &record)
    {
        using db::query::ThreadRemove;
        using db::query::ThreadRemoveResult;

        auto query = std::make_unique<ThreadRemove>(record.ID);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);
        task->setCallback([this, threadId = record.ID](auto response) {
            const auto result = dynamic_cast<ThreadRemoveResult *>(response);
            if ((result != nullptr) && result->success()) {
                switchWindow(gui::name::window::main_window);
                return true;
            }
            LOG_ERROR("ThreadRemove id=%" PRIu32 " failed", threadId);
            return false;
        });
        task->execute(this, this);
        return true;
    }

    bool ApplicationMessages::removeSms(const SMSRecord &record)
    {
        LOG_DEBUG("Removing sms: %" PRIu32, record.ID);
        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{record.body,
                                "phonebook_contact_delete_trashcan",
                                utils::translate("app_messages_message_delete_confirmation"),
                                "",
                                [this, record] { return onRemoveSmsConfirmed(record); }});
        switchWindow(gui::name::window::dialog_yes_no, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
        return true;
    }

    bool ApplicationMessages::onRemoveSmsConfirmed(const SMSRecord &record)
    {
        using db::query::SMSRemove;
        using db::query::SMSRemoveResult;
        using db::query::ThreadGetByID;
        using db::query::ThreadGetByIDResult;

        auto query = std::make_unique<SMSRemove>(record.ID);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMS);
        task->setCallback([this, record](auto response) {
            auto result = dynamic_cast<SMSRemoveResult *>(response);
            if (result != nullptr && result->getResults()) {
                auto query = std::make_unique<ThreadGetByID>(record.threadID);
                auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);
                task->setCallback([this](auto response) {
                    const auto result = dynamic_cast<ThreadGetByIDResult *>(response);
                    if (result != nullptr) {
                        const auto thread = result->getRecord();
                        if (thread.has_value()) {
                            this->switchWindow(gui::name::window::thread_view);
                        }
                        else {
                            this->switchWindow(gui::name::window::main_window);
                        }
                        return true;
                    }
                    return false;
                });
                task->execute(this, this);
                return true;
            }
            LOG_ERROR("sSMSRemove id=%" PRIu32 " failed", record.ID);
            return false;
        });
        task->execute(this, this);
        return true;
    }

    bool ApplicationMessages::searchEmpty(const std::string &query)
    {
        gui::DialogMetadata meta;
        meta.icon                        = "search_big";
        meta.text                        = utils::translate("app_messages_thread_no_result");
        meta.title                       = utils::translate("common_results_prefix") + query;
        auto data                        = std::make_unique<gui::DialogMetadataMessage>(meta);
        data->ignoreCurrentWindowOnStack = true;
        switchWindow(gui::name::window::dialog, std::make_unique<gui::DialogMetadataMessage>(meta));

        return true;
    }

    bool ApplicationMessages::showSearchResults(const UTF8 &title, const UTF8 &search_text)
    {
        switchWindow(gui::name::window::search_results, std::make_unique<SMSTextToSearch>(search_text, title));
        return true;
    }

    bool ApplicationMessages::showNotification(std::function<bool()> action,
                                               const std::string &notification,
                                               bool ignoreCurrentWindowOnStack)
    {
        gui::DialogMetadata meta;
        meta.icon                              = "info_big_circle_W_G";
        meta.text                              = notification;
        meta.action                            = action;
        auto switchData                        = std::make_unique<gui::DialogMetadataMessage>(meta);
        switchData->ignoreCurrentWindowOnStack = ignoreCurrentWindowOnStack;
        switchWindow(gui::name::window::dialog_confirm, std::move(switchData));
        return true;
    }

    bool ApplicationMessages::updateDraft(SMSRecord &record, const UTF8 &body)
    {
        assert(!body.empty()); // precondition check.

        record.body = body;
        record.date = std::time(nullptr);

        using db::query::SMSUpdate;
        const auto [succeed, _] =
            DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSUpdate>(record));
        return succeed;
    }

    std::pair<SMSRecord, bool> ApplicationMessages::createDraft(const utils::PhoneNumber::View &number,
                                                                const UTF8 &body)
    {
        assert(!body.empty()); // precondition check.

        SMSRecord record;
        record.number = number;
        record.body   = body;
        record.type   = SMSType::DRAFT;
        record.date   = std::time(nullptr);

        using db::query::SMSAdd;
        const auto [success, _] =
            DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSAdd>(record));
        return std::make_pair(record, success);
    }

    bool ApplicationMessages::removeDraft(const SMSRecord &record)
    {
        using db::query::SMSRemove;
        const auto [succeed, _] =
            DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSRemove>(record.ID));
        return succeed;
    }

    bool ApplicationMessages::sendSms(const utils::PhoneNumber::View &number, const UTF8 &body)
    {
        if (number.getEntered().size() == 0 || body.length() == 0) {
            LOG_WARN("Number or sms body is empty");
            return false;
        }
        SMSRecord record;
        record.number = number;
        record.body   = body;
        record.type   = SMSType::QUEUED;
        record.date   = std::time(nullptr);

        using db::query::SMSAdd;
        const auto [succeed, _] =
            DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSAdd>(record));
        return succeed;
    }

    bool ApplicationMessages::resendSms(const SMSRecord &record)
    {
        auto resendRecord = record;
        resendRecord.type = SMSType::QUEUED;
        resendRecord.date = std::time(nullptr); // update date sent - it will display an old, failed sms at
                                                // the the bottom, but this is correct

        using db::query::SMSUpdate;
        const auto [succeed, _] =
            DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSUpdate>(resendRecord));
        return succeed;
    }

    bool ApplicationMessages::handleSendSmsFromThread(const utils::PhoneNumber::View &number, const UTF8 &body)
    {
        if (!sendSms(number, body)) {
            return false;
        }

        return true;
    }

    bool ApplicationMessages::newMessageOptions(const std::string &requestingWindow, gui::Text *text)
    {
        LOG_INFO("New message options for %s", requestingWindow.c_str());
        auto opts = std::make_unique<gui::OptionsWindowOptions>(newMessageWindowOptions(this, requestingWindow, text));
        switchWindow(utils::translate("app_phonebook_options_title"), std::move(opts));
        return true;
    }

} /* namespace app */
