// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"

#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "OptionsWindow.hpp"
#include "application-messages/data/SMSTextToSearch.hpp"
#include "windows/MessagesMainWindow.hpp"
#include "windows/NewMessage.hpp"
#include "windows/OptionsMessages.hpp"
#include "windows/SMSThreadViewWindow.hpp"
#include "windows/SearchStart.hpp"
#include "windows/SMSTemplatesWindow.hpp"
#include "windows/SearchResults.hpp"

#include <MessageType.hpp>
#include <Dialog.hpp>
#include <i18n/i18n.hpp>
#include <memory>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <OptionWindow.hpp>

#include <module-db/queries/messages/sms/QuerySMSAdd.hpp>
#include <module-db/queries/messages/sms/QuerySMSRemove.hpp>
#include <module-db/queries/messages/sms/QuerySMSUpdate.hpp>
#include <module-db/queries/messages/threads/QueryThreadGetByID.hpp>
#include <module-db/queries/messages/threads/QueryThreadRemove.hpp>
#include <module-db/queries/phonebook/QueryContactGetByID.hpp>

#include <cassert>
#include <time/time_conversion.hpp>
#include <messages/OptionsWindow.hpp>

namespace app
{
    ApplicationMessages::ApplicationMessages(std::string name, std::string parent, StartInBackground startInBackground)
        : Application(name, parent, startInBackground, 4096 * 2)
    {
        busChannels.push_back(sys::BusChannels::ServiceDBNotifications);
        addActionReceiver(manager::actions::CreateSms, [this](auto &&data) {
            switchWindow(gui::name::window::new_sms, std::move(data));
            return msgHandled();
        });
        addActionReceiver(manager::actions::ShowSmsTemplates, [this](auto &&data) {
            switchWindow(gui::name::window::sms_templates, std::move(data));
            return msgHandled();
        });
    }

    ApplicationMessages::~ApplicationMessages()
    {}

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

        // this variable defines whether message was processed.
        bool handled = false;

        // handle database response
        if (resp != nullptr) {
            handled = true;
            switch (resp->responseTo) {
            case MessageType::DBThreadGetLimitOffset:
                [[fallthrough]];
            case MessageType::DBSMSTemplateGetLimitOffset:
                if (getCurrentWindow()->onDatabaseMessage(resp)) {
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                }
                break;
            case MessageType::DBQuery:
                if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp)) {
                    auto result = queryResponse->getResult();
                    if (result && result->hasListener()) {
                        if (result->handle()) {
                            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                        }
                    }
                }
                break;
            default:
                break;
            }
        }

        if (handled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationMessages::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();
        setActiveWindow(gui::name::window::main_window);
        return ret;
    }

    void ApplicationMessages::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::MessagesMainWindow>(app);
        });
        windowsFactory.attach(gui::name::window::thread_view, [](Application *app, const std::string &name) {
            return std::make_unique<gui::SMSThreadViewWindow>(app);
        });
        windowsFactory.attach(gui::name::window::new_sms, [](Application *app, const std::string &name) {
            return std::make_unique<gui::NewMessageWindow>(app);
        });
        windowsFactory.attach(
            utils::localize.get("app_phonebook_options_title"),
            [](Application *app, const std::string &name) { return std::make_unique<gui::OptionWindow>(app, name); });
        windowsFactory.attach(gui::name::window::dialog, [](Application *app, const std::string &name) {
            return std::make_unique<gui::Dialog>(app, name);
        });
        windowsFactory.attach(gui::name::window::dialog_confirm, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });
        windowsFactory.attach(gui::name::window::dialog_yes_no, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogYesNo>(app, name);
        });
        windowsFactory.attach(gui::name::window::thread_sms_search, [](Application *app, const std::string &name) {
            return std::make_unique<gui::SMSSearch>(app);
        });
        windowsFactory.attach(gui::name::window::sms_templates, [](Application *app, const std::string &name) {
            return std::make_unique<gui::SMSTemplatesWindow>(app);
        });
        windowsFactory.attach(gui::name::window::search_results, [](Application *app, const std::string &name) {
            return std::make_unique<gui::SearchResults>(app);
        });
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
        query->setQueryListener(db::QueryCallback::fromFunction([this, record](auto response) {
            auto result = dynamic_cast<ContactGetByIDResult *>(response);
            if (result != nullptr) {
                const auto &contact = result->getResult();
                gui::DialogMetadata meta;
                meta.action = [this, record]() { return onRemoveSmsThreadConfirmed(*record); };
                meta.text   = utils::localize.get("app_messages_thread_delete_confirmation");
                meta.title  = contact.getFormattedName();
                meta.icon   = "phonebook_contact_delete_trashcan";
                switchWindow(gui::name::window::dialog_yes_no, std::make_unique<gui::DialogMetadataMessage>(meta));
                return true;
            }
            return false;
        }));
        return DBServiceAPI::GetQuery(this, db::Interface::Name::Contact, std::move(query));
    }

    bool ApplicationMessages::onRemoveSmsThreadConfirmed(const ThreadRecord &record)
    {
        using db::query::ThreadRemove;
        using db::query::ThreadRemoveResult;

        auto query = std::make_unique<ThreadRemove>(record.ID);
        query->setQueryListener(db::QueryCallback::fromFunction([this, threadId = record.ID](auto response) {
            const auto result = dynamic_cast<ThreadRemoveResult *>(response);
            if ((result != nullptr) && result->success()) {
                switchWindow(gui::name::window::main_window);
                return true;
            }
            LOG_ERROR("ThreadRemove id=%" PRIu32 " failed", threadId);
            return false;
        }));

        if (const auto ok = DBServiceAPI::GetQuery(this, db::Interface::Name::SMSThread, std::move(query)); !ok) {
            LOG_ERROR("Unable to query DBServiceAPI");
            return false;
        }
        return true;
    }

    bool ApplicationMessages::removeSms(const SMSRecord &record)
    {
        LOG_DEBUG("Removing sms: %" PRIu32, record.ID);

        gui::DialogMetadata meta;
        meta.action = [this, record] { return onRemoveSmsConfirmed(record); };
        meta.text   = utils::localize.get("app_messages_message_delete_confirmation");
        meta.title  = record.body;
        meta.icon   = "phonebook_contact_delete_trashcan";

        switchWindow(gui::name::window::dialog_yes_no, std::make_unique<gui::DialogMetadataMessage>(meta));
        return true;
    }

    bool ApplicationMessages::onRemoveSmsConfirmed(const SMSRecord &record)
    {
        using db::query::SMSRemove;
        using db::query::SMSRemoveResult;
        using db::query::ThreadGetByID;
        using db::query::ThreadGetByIDResult;

        auto query = std::make_unique<SMSRemove>(record.ID);
        query->setQueryListener(db::QueryCallback::fromFunction([this, record](auto response) {
            auto result = dynamic_cast<SMSRemoveResult *>(response);
            if (result != nullptr && result->getResults()) {
                auto query = std::make_unique<ThreadGetByID>(record.threadID);
                query->setQueryListener(db::QueryCallback::fromFunction([this](auto response) {
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
                }));
                return DBServiceAPI::GetQuery(this, db::Interface::Name::SMSThread, std::move(query));
            }
            LOG_ERROR("sSMSRemove id=%" PRIu32 " failed", record.ID);
            return false;
        }));

        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::move(query));
    }

    bool ApplicationMessages::searchEmpty(const std::string &query)
    {
        gui::DialogMetadata meta;
        meta.icon  = "search_big";
        meta.text  = utils::localize.get("app_messages_thread_no_result");
        meta.title = utils::localize.get("common_results_prefix") + query;
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

    bool ApplicationMessages::showNotification(std::function<bool()> action, bool ignoreCurrentWindowOnStack)
    {
        gui::DialogMetadata meta;
        meta.icon   = "info_big_circle_W_G";
        meta.text   = utils::localize.get("app_messages_no_sim");
        meta.action = action;
        auto switchData                        = std::make_unique<gui::DialogMetadataMessage>(meta);
        switchData->ignoreCurrentWindowOnStack = ignoreCurrentWindowOnStack;
        switchWindow(gui::name::window::dialog_confirm, std::move(switchData));
        return true;
    }

    bool ApplicationMessages::updateDraft(SMSRecord &record, const UTF8 &body)
    {
        assert(!body.empty()); // precondition check.

        record.body = body;
        record.date = utils::time::getCurrentTimestamp().getTime();

        using db::query::SMSUpdate;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSUpdate>(record));
    }

    std::pair<SMSRecord, bool> ApplicationMessages::createDraft(const utils::PhoneNumber::View &number,
                                                                const UTF8 &body)
    {
        assert(!body.empty()); // precondition check.

        SMSRecord record;
        record.number = number;
        record.body   = body;
        record.type   = SMSType::DRAFT;
        record.date   = utils::time::getCurrentTimestamp().getTime();

        using db::query::SMSAdd;
        const auto success = DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSAdd>(record));
        return std::make_pair(record, success);
    }

    bool ApplicationMessages::removeDraft(const SMSRecord &record)
    {
        using db::query::SMSRemove;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSRemove>(record.ID));
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
        record.date   = utils::time::getCurrentTimestamp().getTime();

        using db::query::SMSAdd;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSAdd>(record));
    }

    bool ApplicationMessages::resendSms(const SMSRecord &record)
    {
        auto resendRecord = record;
        resendRecord.type = SMSType::QUEUED;
        resendRecord.date =
            utils::time::getCurrentTimestamp().getTime(); // update date sent - it will display an old, failed sms at
                                                          // the the bottom, but this is correct

        using db::query::SMSUpdate;
        return DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<SMSUpdate>(resendRecord));
    }

    bool ApplicationMessages::handleSendSmsFromThread(const utils::PhoneNumber::View &number, const UTF8 &body)
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
        LOG_INFO("New message options for %s", requestingWindow.c_str());
        auto opts = std::make_unique<gui::OptionsWindowOptions>(newMessageWindowOptions(this, requestingWindow, text));
        switchWindow(utils::localize.get("app_phonebook_options_title"), std::move(opts));
        return true;
    }

} /* namespace app */
