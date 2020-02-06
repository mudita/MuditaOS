#include "UiCommon.hpp"
#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/windows/ThreadViewWindow.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <i18/i18.hpp>

namespace app
{
    bool call(Application *app, const ContactRecord &contact)
    {
        if (app == nullptr)
        {
            LOG_ERROR("app = nullptr");
            return false;
        }

        if (contact.numbers.size() != 0)
        {
            std::unique_ptr<ExecuteCallData> data = std::make_unique<ExecuteCallData>(contact.numbers[0].numberE164.c_str());
            return sapm::ApplicationManager::messageSwitchApplication(app, name_call, window::name_call, std::move(data));
        }
        else
        {
            LOG_ERROR("No contact numbers!");
            return false;
        }
    }

    gui::Option callOption(Application *app, const ContactRecord &contact, bool active)
    {
        return {UTF8(utils::localize.get("sms_call_text")) + contact.primaryName, [app, contact, active](gui::Item &item) {
                    if (active)
                    {
                        LOG_DEBUG("Try call!");
                        return call(app, contact);
                    }
                    else
                    {
                        LOG_ERROR("Inactive call option");
                        return false;
                    }
                }};
    }

    gui::Option contactDetails(Application *app, const ContactRecord &contact)
    {
        if (app == nullptr)
        {
            LOG_ERROR("app = nullptr");
            return gui::Option();
        }
        auto foo = [=](gui::Item &item) {
            return sapm::ApplicationManager::messageSwitchApplication(
                app, name_phonebook, "Contact", std::make_unique<PhonebookItemData>(std::shared_ptr<ContactRecord>(new ContactRecord(contact))));
        };
        return {utils::localize.get("sms_contact_details"), foo};
    }

    bool sms(Application *app, const ContactRecord &contact)
    {
        // TODO: alek: looks like only valid from SMS app
        if (app == nullptr)
        {
            LOG_ERROR("app = nullptr");
            return false;
        }
        // TODO return to current application doesn't change application window >_>
        auto param = std::shared_ptr<ContactRecord>(new ContactRecord(contact));
        return app->switchWindow(gui::name::window::thread_view, std::make_unique<SMSSendRequest>(param));
    }

    bool addContact(Application *app, const ContactRecord &contact)
    {
        if (app == nullptr)
        {
            LOG_ERROR("app = nullptr");
            return false;
        }
        return sapm::ApplicationManager::messageSwitchApplication(
            app, name_phonebook, "New", std::make_unique<PhonebookItemData>(std::shared_ptr<ContactRecord>(new ContactRecord(contact))));
    }
} // namespace app