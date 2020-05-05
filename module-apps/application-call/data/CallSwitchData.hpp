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
            INCOMMING_CALL,
            EXECUTE_CALL
        };
        static const inline std::string descriptionStr = "CallSwitchData";

      protected:
        Type type = Type::UNDEFINED;
        utils::PhoneNumber::View phoneNumber;

      public:
        CallSwitchData(const utils::PhoneNumber::View &phoneNumber, Type type = Type::UNDEFINED)
            : SwitchData(descriptionStr), type(type), phoneNumber(phoneNumber){};
        virtual ~CallSwitchData(){};

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
        static const inline std::string descriptionStr = "EnterNumberSwitchData";

        EnterNumberData(const std::string &phoneNumber) : SwitchData(descriptionStr), phoneNumber(phoneNumber)
        {}
        virtual ~EnterNumberData(){};

        const std::string &getPhoneNumber() const
        {
            return phoneNumber;
        }
    };

    class IncommingCallData : public CallSwitchData
    {
      public:
        IncommingCallData(const utils::PhoneNumber::View &phoneNumber)
            : CallSwitchData(phoneNumber, CallSwitchData::Type::INCOMMING_CALL){};
        virtual ~IncommingCallData(){};
    };

    class ExecuteCallData : public CallSwitchData
    {
      public:
        ExecuteCallData(const utils::PhoneNumber::View &phoneNumber)
            : CallSwitchData(phoneNumber, app::CallSwitchData::Type::EXECUTE_CALL){};

        virtual ~ExecuteCallData(){};
    };

} /* namespace app */
