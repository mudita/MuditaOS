#pragma once

#include "Common.hpp"
#include "LogOutput.hpp"
#include <string>
#include <vector>
#include <stdint.h>
#include <memory>


namespace sys
{

    class Service;
    class ResponseMessage;
    class Message;


    enum class SystemMessageType{
        Ping,
        Wakeup,
        GoSleep,
        Start,
        Exit
    };

    using Message_t = std::shared_ptr<Message>;
    using MessageRet_t = std::pair<ReturnCodes ,std::shared_ptr<Message>>;


    MessageRet_t CreateMessageRet(ReturnCodes retCode,Message_t msg);

    class Message
    {

    public:

        enum class TransmissionType{
            Unicast,
            Multicast,
            Broadcaast
        };

        enum class Type{
            System,
            Data,
            Response
        };


        Message()
        {
#ifdef UNIT_TESTS
            unitestsMsgInstancesCount++;
#endif
        }

        Message(BusChannels chan)
        : channel(chan)
        {
#ifdef UNIT_TESTS
            unitestsMsgInstancesCount++;
#endif
        }

        virtual ~Message(){
#ifdef UNIT_TESTS
            unitestsMsgInstancesCount--;
#endif
        }

        virtual Message_t Execute(Service* service) = 0;

        Type type;
        TransmissionType transType;
        BusChannels channel;
        std::string sender;
        uint64_t id;
        uint64_t uniID;

#ifdef UNIT_TESTS
        static uint32_t unitestsMsgInstancesCount;
#endif
    };

    class SystemMessage : public Message
    {
    public:

        SystemMessage(SystemMessageType sysMsgType):
            Message(BusChannels ::System),
            sysMsgType(sysMsgType)
        {
            type = Type::System;
        }

        Message_t Execute(Service* service) override;

        SystemMessageType sysMsgType;
    };

    class DataMessage : public Message
    {
    public:

        DataMessage(){
            type = Type::Data;
        }

        DataMessage(BusChannels channel): Message(channel)
        {
            type = Type::Data;
        }

        Message_t Execute(Service* service) override;
    };

    class ResponseMessage : public Message
    {
    public:

        ResponseMessage(ReturnCodes retCode = ReturnCodes::Success): Message(),retCode(retCode)
        {
            type = Type::Response;
        }

        ReturnCodes retCode;

        Message_t Execute(Service* service) override;
    };


}
