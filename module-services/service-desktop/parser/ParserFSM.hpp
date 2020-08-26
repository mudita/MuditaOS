#pragma once
#include "ParserUtils.hpp"
#include "json/json11.hpp"
#include "MessageHandler.hpp"

namespace parserFSM
{
    enum class State
    {
        NoMsg,
        ReceivedPartialHeader,
        ReceivedPartialPayload,
        ReceivedPayload,
    };

    class StateMachine
    {
      public:
        StateMachine(sys::Service *OwnerService);
        void processMessage(std::string &msg);
        State getCurrentState()
        {
            return state;
        };

      private:
        std::string *receivedMsgPtr = nullptr;
        parserFSM::State state      = State::NoMsg;
        std::string payload;
        std::string header;
        unsigned long payloadLength   = 0;
        sys::Service *OwnerServicePtr = nullptr;

        void parseHeader();
        void parsePartialHeader();
        void parseNewMessage();
        void parsePartialMessage();
        void parsePayload();
    };
} // namespace parserFSM
