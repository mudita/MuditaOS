// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SwitchData.hpp"

#include <PhoneNumber.hpp>

#include <string>

namespace app
{

    class CallSwitchData : public gui::SwitchData
    {
      public:
        enum class Type
        {
            UNDEFINED,
            INCOMING_CALL,
            EXECUTE_CALL
        };
        static constexpr auto descriptionStr = "CallSwitchData";

      protected:
        Type type = Type::UNDEFINED;
        utils::PhoneNumber::View phoneNumber;

      public:
        CallSwitchData(const utils::PhoneNumber::View &phoneNumber, Type type = Type::UNDEFINED)
            : SwitchData(descriptionStr), type(type), phoneNumber(phoneNumber){};

        const Type &getType() const
        {
            return type;
        };
        const utils::PhoneNumber::View &getPhoneNumber() const
        {
            return phoneNumber;
        };
    };

    class EnterNumberData : public gui::SwitchData
    {
        std::string phoneNumber;

      public:
        static constexpr auto descriptionStr = "EnterNumberSwitchData";

        EnterNumberData(const std::string &phoneNumber) : SwitchData(descriptionStr), phoneNumber(phoneNumber)
        {}

        const std::string &getPhoneNumber() const
        {
            return phoneNumber;
        }
    };

    class ExecuteCallData : public CallSwitchData
    {
      public:
        ExecuteCallData(const utils::PhoneNumber::View &phoneNumber)
            : CallSwitchData(phoneNumber, app::CallSwitchData::Type::EXECUTE_CALL){};
    };
} /* namespace app */
