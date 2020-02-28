#pragma once

#include "Application.hpp"
#include <OptionWindow.hpp>
#include <service-db/api/DBServiceAPI.hpp>

namespace app
{
    enum class CallOperation
    {
        ExecuteCall,
        PrepareCall
    };
    bool call(Application *app, CallOperation callOperation, const ContactRecord &contact);
    bool call(Application *app, CallOperation callOperation, const std::string &number);
    bool call(Application *app, CallOperation callOperation, uint32_t key);

    enum class SmsOperation
    {
        Add
    };
    bool sms(Application *app, SmsOperation smsOperation, const ContactRecord &contact);
    bool sms(Application *app, SmsOperation smsOperation, const std::string &number);

    enum class ContactOperation
    {
        Add,
        Details
    };
    // TODO use contact here
    bool contact(Application *app, ContactOperation contactOperation, const ContactRecord &contact);
    bool contact(Application *app, ContactOperation contactOperation, const std::string &number);
} // namespace app
