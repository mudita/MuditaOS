#pragma once

#include "Application.hpp"

#include <OptionWindow.hpp>
#include <PhoneNumber.hpp>
#include <service-db/api/DBServiceAPI.hpp>

namespace app
{
    enum class CallOperation
    {
        ExecuteCall,
        PrepareCall
    };

    /// @brief requests selected call operation
    ///
    /// @param app - requesting application
    /// @param CallOperation - selected call operation.
    ///
    /// @parem contact - contact record on which the operation is requested
    ///
    /// @return true if succeed
    bool call(Application *app, const ContactRecord &contact);

    /// @brief requests selected call operation
    ///
    /// @param app - requesting application
    /// @param CallOperation - selected call operation.
    ///
    /// @param number - phone number (assigned to contact) on which the operation is requested
    ///
    /// @return true if succeed
    bool call(Application *app, const std::string &number);

    /// @brief requests selected call operation
    ///
    /// @param app - requesting application
    /// @param CallOperation - selected call operation.
    ///
    /// @param number - phone number (assigned to contact) on which the operation is requested
    ///
    /// @return true if succeed
    bool call(Application *app, const utils::PhoneNumber::View &number);

    /// @brief prepares for a call (displays number to the user)
    ///
    /// @param app - requesting application
    /// @param number - phone number on which the operation is requested
    ///
    /// @return true if succeed
    bool prepare_call(Application *app, const std::string &number);

    enum class SmsOperation
    {
        Add
    };

    /// @brief requests selected sms operation
    ///
    /// @param app - requesting application
    /// @param smsOperation - selected sms operation.
    ///
    /// @parem contact - contact record on which the operation is requested
    ///
    /// @return true if succeed
    bool sms(Application *app, SmsOperation smsOperation, const ContactRecord &contact);

    /// @brief requests selected sms operation
    ///
    /// @param app - requesting application
    /// @param smsOperation - selected sms operation.
    ///
    /// @parem number - phone number (assigned to contact) on which the operation is requested
    ///
    /// @return true if succeed
    bool sms(Application *app, SmsOperation smsOperation, const std::string &number);

    enum class ContactOperation
    {
        Add,
        Details,
        Edit
    };

    /// @brief requests selected contact operation
    ///
    /// @param app - requesting application
    /// @param contactOperation - selected contact operation.
    ///
    /// @parem contact - contact record on which the operation is requested
    ///
    /// @return true if succeed
    bool contact(Application *app, ContactOperation contactOperation, const ContactRecord &contact);

    /// @brief requests selected contact operation
    ///
    /// @param app - requesting application
    /// @param contactOperation - selected contact operation.
    ///
    /// @parem number - phone number (assigned to contact) on which the operation is requested
    ///
    /// @return true if succeed
    bool contact(Application *app, ContactOperation contactOperation, const std::string &number);

    /// @brief requests selected contact operation
    ///
    /// @param app - requesting application
    /// @param contactOperation - selected contact operation. @note ContactOperation::Add is no valid in this case
    ///
    /// @parem contactId - id of contact on which the operation is requested
    ///
    /// @return true if succeed
    bool contact(Application *app, ContactOperation contactOperation, uint32_t contactId);
} // namespace app
