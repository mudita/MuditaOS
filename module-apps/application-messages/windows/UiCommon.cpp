#include "UiCommon.hpp"
#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <i18/i18.hpp>

gui::Option callOption(app::ApplicationMessages *app, ContactRecord &contact, bool active)
{
    return {UTF8(utils::localize.get("sms_call_text")) + contact.primaryName, [=](gui::Item &item) {
                if (!active)
                {
                    return false;
                }
                else
                {
                    std::unique_ptr<app::ExecuteCallData> data = std::make_unique<app::ExecuteCallData>(contact.numbers[0].numberE164.c_str());
                    return sapm::ApplicationManager::messageSwitchApplication(app, app::name_call, "CallWindow", std::move(data));
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
