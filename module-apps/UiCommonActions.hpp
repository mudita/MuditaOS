// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
    auto call(Application *app, const ContactRecord &contact) -> bool;

    /// @brief requests selected call operation
    ///
    /// @param app - requesting application
    /// @param CallOperation - selected call operation.
    ///
    /// @param number - phone number (assigned to contact) on which the operation is requested
    ///
    /// @return true if succeed
    auto call(Application *app, const utils::PhoneNumber::View &number) -> bool;

    /// @brief prepares for a call (displays number to the user)
    ///
    /// @param app - requesting application
    /// @param number - phone number on which the operation is requested
    ///
    /// @return true if succeed
    auto prepareCall(Application *app, const std::string &number) -> bool;

    enum class SmsOperation
    {
        New,
        Template
    };

    /// @brief requests selected sms operation
    ///
    /// @param app - requesting application
    /// @param smsOperation - selected sms operation.
    ///
    /// @param number - phone number (assigned to contact) on which the operation is requested
    ///
    /// @param textData - text message which should be sent as sms
    ///
    /// @return true if succeed
    auto sms(Application *app,
             SmsOperation smsOperation,
             const utils::PhoneNumber::View &number,
             const UTF8 textData = "") -> bool;

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
    auto contact(Application *app, ContactOperation contactOperation, const ContactRecord &contact) -> bool;

    /// @brief requests selected contact operation
    ///
    /// @param app - requesting application
    /// @param contactOperation - selected contact operation.
    ///
    /// @parem number - phone number (assigned to contact) on which the operation is requested
    ///
    /// @return true if succeed
    auto contact(Application *app, ContactOperation contactOperation, const std::string &number) -> bool;

    /// @brief requests selected contact operation
    ///
    /// @param app - requesting application
    /// @param contactOperation - selected contact operation. @note ContactOperation::Add is no valid in this case
    ///
    /// @parem contactId - id of contact on which the operation is requested
    ///
    /// @return true if succeed
    auto contact(Application *app, ContactOperation contactOperation, uint32_t contactId) -> bool;
} // namespace app
