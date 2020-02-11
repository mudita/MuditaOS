#include "UiCommon.hpp"
#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <i18/i18.hpp>

bool call(app::ApplicationMessages *app, const ContactRecord contact)
{
    if (contact.numbers.size() != 0)
    {
        std::unique_ptr<app::ExecuteCallData> data = std::make_unique<app::ExecuteCallData>(contact.numbers[0].numberE164.c_str());
        return sapm::ApplicationManager::messageSwitchApplication(app, app::name_call, "CallWindow", std::move(data));
    }
    else
    {
        LOG_ERROR("No contact numbers!");
        return false;
    }
}

gui::Option callOption(app::ApplicationMessages *app, const ContactRecord contact, bool active)
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

#include "application-phonebook/ApplicationPhonebook.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <application-phonebook/data/PhonebookItemData.hpp>

gui::Option contactDetails(app::ApplicationMessages *app, ContactRecord &contact)
{
    auto foo = [=](gui::Item &item) {
        return sapm::ApplicationManager::messageSwitchApplication(
            app, app::name_phonebook, "Contact", std::make_unique<PhonebookItemData>(std::shared_ptr<ContactRecord>(new ContactRecord(contact))));
    };
    return {utils::localize.get("sms_contact_details"), foo};
}

#include "../data/SMSdata.hpp"
#include "ThreadViewWindow.hpp"

bool sms(app::ApplicationMessages *app, const ContactRecord contact)
{
    // TODO return to current application doesn't change application window >_>
    auto param = std::shared_ptr<ContactRecord>(new ContactRecord(contact));
    return app->switchWindow(gui::name::window::thread_view, std::make_unique<SMSSendRequest>(param));
}

bool addContact(app::ApplicationMessages *app, const ContactRecord contact)
{
    return sapm::ApplicationManager::messageSwitchApplication(app, app::name_phonebook, "New", nullptr);
}
