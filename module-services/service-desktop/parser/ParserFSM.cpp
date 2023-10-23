// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ParserFSM.hpp"

#include <log/log.hpp>
#include <memory>
#include <string>

#include <Timers/TimerFactory.hpp>
#include "MessageHandler.hpp"
#include <endpoints/EndpointFactory.hpp>
#include <endpoints/message/Common.hpp>

namespace
{
    constexpr auto receiveMsgTimerDelayMs = std::chrono::milliseconds{1000 * 5};
    constexpr auto parserTimerName        = "parserTimer";

} // namespace

namespace sdesktop::endpoints
{

    StateMachine::StateMachine(sys::Service *OwnerService)
        : OwnerServicePtr(OwnerService),
          parserTimer{sys::TimerFactory::createSingleShotTimer(
              OwnerService, parserTimerName, receiveMsgTimerDelayMs, [this](sys::Timer & /*timer*/) { resetParser(); })}
    {}

    void StateMachine::processMessage(std::string &&msg)
    {
        receivedMsg = std::move(msg);

        restartTimer();

        switch (state) {
        case State::NoMsg:
            parseHeader();
            break;
        case State::ReceivedPartialHeader:
            parsePartialHeader();
            break;
        case State::ReceivedPartialPayload:
            parsePartialMessage();
            break;

        default:
            break;
        }
    }

    void StateMachine::resetParser()
    {
        payload.clear();
        header.clear();
        payloadLength = 0;

        setState(State::NoMsg);
        LOG_DEBUG("Parser state reset");
    }

    void StateMachine::restartTimer()
    {
        parserTimer.restart(receiveMsgTimerDelayMs);
    }

    void StateMachine::parseHeader()
    {
        payload.clear();
        header.clear();
        payloadLength = 0;

        auto messageStart = receivedMsg.find(message::endpointChar);
        if (messageStart == std::string::npos) {
            LOG_ERROR("This is not a valid endpoint message! Type=%c", receivedMsg.at(0));
            return;
        }

        if (receivedMsg.size() < message::size_header) // header divided in few parts
        {
            setState(State::ReceivedPartialHeader);
            header.append(receivedMsg); // append to whole header string
            return;
        }

        header        = message::getHeader(receivedMsg);
        payloadLength = message::calcPayloadLength(header);
        if (payloadLength == 0) // failed to obtain payload length from msg
        {
            LOG_ERROR("Damaged header!");
            setState(State::NoMsg);
            return;
        }

        message::removeHeader(receivedMsg);
        parseNewMessage();
    }

    void StateMachine::parsePartialHeader()
    {
        auto previousHeaderLength = header.size();
        auto missingHeaderLength  = message::size_header - previousHeaderLength;

        if (receivedMsg.size() >= missingHeaderLength) // rest of the message is here
        {
            header.append(receivedMsg.substr(0, missingHeaderLength));
            LOG_DEBUG("Header: %s\n", header.c_str());
            payloadLength = message::calcPayloadLength(header);

            message::eraseFront(receivedMsg, missingHeaderLength);

            parseNewMessage();
        }
        else // the message is even longer :(
        {
            header.append(receivedMsg);
        }
    }

    void StateMachine::parseNewMessage()
    {
        if (receivedMsg.size() >= payloadLength) {

            payload = message::extractPayload(receivedMsg, payloadLength);

            parsePayload();

            if (receivedMsg.size() > payloadLength) { // contains part of new header
                message::eraseFront(receivedMsg, payloadLength);
                parseHeader();
            }
        }
        else // message divided in 2 or more packets
        {
            payload = receivedMsg.substr(0, std::string::npos); // take rest of the message
            setState(State::ReceivedPartialPayload);
        }
    }

    void StateMachine::parsePartialMessage()
    {
        auto previousPayloadLength = payload.size();
        auto missingPayloadLength  = payloadLength - previousPayloadLength;

        if (receivedMsg.size() >= missingPayloadLength) // rest of the message is here
        {
            payload.append(message::extractPayload(receivedMsg, missingPayloadLength));

            parsePayload();

            if (receivedMsg.size() > missingPayloadLength) {
                message::eraseFront(receivedMsg, missingPayloadLength);
                parseHeader();
            }
        }
        else // the message is even longer
        {
            payload.append(receivedMsg);
        }
    }

    void StateMachine::parsePayload()
    {
        if (payload.empty()) {
            LOG_ERROR("Empty payload!");
            setState(State::NoMsg);
            return;
        }

        messageHandler->parseMessage(payload);

        if (!messageHandler->isValid() || messageHandler->isJSONNull()) {
            LOG_ERROR("Error parsing JSON");
            setState(State::NoMsg);
            return;
        }

        messageHandler->processMessage();
        setState(State::NoMsg);
        parserTimer.stop();
    }

    void StateMachine::setMessageHandler(std::unique_ptr<MessageHandler> handler)
    {
        messageHandler = std::move(handler);
    }

} // namespace sdesktop::endpoints
