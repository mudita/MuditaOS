/*
 * @file CallSwitchData.h
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_CALL_DATA_CALLSWITCHDATA_HPP_
#define MODULE_APPS_APPLICATION_CALL_DATA_CALLSWITCHDATA_HPP_

#include <string>
#include "SwitchData.hpp"

namespace app
{

    class CallSwitchData : public gui::SwitchData
    {
      public:
        enum class Type
        {
            UNDEFINED,
            ENTER_NUMBER,
            INCOMMING_CALL,
            EXECUTE_CALL
        };
        static const inline std::string descriptionStr     = "CallSwitchData";
        static const inline std::string defaultPhoneNumber = "undefined";

      protected:
        Type type = Type::UNDEFINED;
        std::string phoneNumber;

      public:
        CallSwitchData(const std::string &number = defaultPhoneNumber, Type type = Type::UNDEFINED)
            : SwitchData(descriptionStr), type{type}, phoneNumber{number} {};
        virtual ~CallSwitchData(){};

        const Type &getType() const
        {
            return type;
        };
        const std::string &getPhoneNumber() const
        {
            return phoneNumber;
        };
    };

    class EnterNumberData : public CallSwitchData
    {
      public:
        EnterNumberData(const std::string &number = "") : CallSwitchData(number, CallSwitchData::Type::ENTER_NUMBER){};
        virtual ~EnterNumberData(){};
    };

    class IncommingCallData : public CallSwitchData
    {
      public:
        IncommingCallData(const std::string &number = "")
            : CallSwitchData(number, CallSwitchData::Type::INCOMMING_CALL){};
        virtual ~IncommingCallData(){};
    };

    class ExecuteCallData : public CallSwitchData
    {
      protected:
        std::string phoneNumber;

      public:
        ExecuteCallData(const std::string &number = "")
            : CallSwitchData(number, app::CallSwitchData::Type::EXECUTE_CALL){};
        virtual ~ExecuteCallData(){};
    };

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_CALL_DATA_CALLSWITCHDATA_HPP_ */
