#include "UiCommon.hpp"
#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/windows/ThreadViewWindow.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <cassert>
#include <i18/i18.hpp>
#include <log/log.hpp>

namespace app
{
    bool call(Application *app, CallOperation callOperation, const ContactRecord &contact)
    {
        assert(app != nullptr);

        if (contact.numbers.size() != 0)
        {
            return call(app, callOperation, contact.numbers[0].numberE164);
        }
        else
        {
            LOG_ERROR("No contact numbers!");
            return false;
        }
    }

    bool call(Application *app, CallOperation callOperation, const std::string &e164number)
    {
        assert(app != nullptr);
        const std::string window = window::name_enterNumber;

        std::unique_ptr<CallSwitchData> data;

        switch (callOperation)
        {
        case CallOperation::ExecuteCall: {
            data = std::make_unique<ExecuteCallData>(e164number.c_str());
        }
        break;
        case CallOperation::PrepareCall: {
            data = std::make_unique<EnterNumberData>(e164number.c_str());
        }
        break;
        default: {
            LOG_ERROR("callOperation not supported %d", static_cast<uint32_t>(callOperation));
            return false;
        }
        }
        return sapm::ApplicationManager::messageSwitchApplication(app, name_call, window, std::move(data));
    }

    bool call(Application *app, CallOperation callOperation, uint32_t key)
    {
        return call(app, callOperation, std::to_string(key));
    }

    gui::Option callOption(Application *app, CallOperation callOperation, const ContactRecord &contact)
    {
        assert(app != nullptr);
        return {UTF8(utils::localize.get("sms_call_text")) + contact.primaryName,
                [app, contact, callOperation](gui::Item &item) { return call(app, callOperation, contact); }};
    }

    bool sms(Application *app, SmsOperation smsOperation, const ContactRecord &contact)
    {
        assert(app != nullptr);
        // TODO return to current application doesn't change application window >_>
        auto param = std::shared_ptr<ContactRecord>(new ContactRecord(contact));
        switch (smsOperation)
        {
        case SmsOperation::Add: {
            return sapm::ApplicationManager::messageSwitchApplication(app, name_messages, gui::name::window::thread_view,
                                                                      std::make_unique<SMSSendRequest>(param));
        }
        default: {
            LOG_ERROR("SmsOperation not suppoert %d", static_cast<uint32_t>(smsOperation));
            return false;
        }
        }
    }

    bool sms(Application *app, SmsOperation smsOperation, const std::string &number)
    {
        assert(app != nullptr);
        ContactRecord contactRec;
        contactRec.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(number, number)});
        return sms(app, smsOperation, contactRec);
    }

    bool contact(Application *app, ContactOperation contactOperation, const ContactRecord &contact)
    {
        assert(app != nullptr);
        switch (contactOperation)
        {
        case ContactOperation::Add: {
            return sapm::ApplicationManager::messageSwitchApplication(
                app, name_phonebook, "New", std::make_unique<PhonebookItemData>(std::shared_ptr<ContactRecord>(new ContactRecord(contact))));
        }
        break;
        case ContactOperation::Details: {
            return sapm::ApplicationManager::messageSwitchApplication(
                app, name_phonebook, "Contact", std::make_unique<PhonebookItemData>(std::shared_ptr<ContactRecord>(new ContactRecord(contact))));
        }
        break;
        default: {
            LOG_ERROR("ContactOperation not supported %d", static_cast<uint32_t>(contactOperation));
            return false;
        }
        }
    }

    bool contact(Application *app, ContactOperation contactOperation, const std::string &number)
    {
        assert(app != nullptr);

        ContactRecord contactRecord;
        contactRecord.numbers = std::vector<ContactRecord::Number>({ContactRecord::Number(number, number)});

        return contact(app, contactOperation, contactRecord);
    }

    gui::Option contactOption(Application *app, ContactOperation contactOperation, const ContactRecord &contactRec)
    {
        assert(app != nullptr);

        return {utils::localize.get("sms_contact_details"), [=](gui::Item &item) { return contact(app, contactOperation, contactRec); }};
    }
} // namespace app
