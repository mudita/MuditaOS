// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessageHandler.hpp"
#include "ParserFSM.hpp"

namespace sys
{
    class Service;
} // namespace sys

#include <service-desktop/ServiceDesktop.hpp>
#include <log.hpp>
#include <memory>
#include <string>

#include "MessageHandler.hpp" // for MessageHandler
#include "ParserUtils.hpp" // for eraseFront, calcPayloadLength, extractPayload, getHeader, removeHeader, size_header, endpointChar, parserFSM

namespace sys
{
    class Service;
}

using namespace parserFSM;

StateMachine::StateMachine(sys::Service *OwnerService) : OwnerServicePtr(OwnerService)
{}

void StateMachine::processMessage(std::string &&msg)
{
    receivedMsg = std::move(msg);

    switch (state) {
    case State::ReceivedPayload:
        state = State::NoMsg;
        [[fallthrough]];
    case State::NoMsg: // first message - empty payload
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
        state = State::ReceivedPartialHeader;
        header.append(receivedMsg); // append to whole header string
        return;
    }

    header        = message::getHeader(receivedMsg);
    payloadLength = message::calcPayloadLength(header);
    if (payloadLength == 0) // failed to obtain payload length from msg
    {
        LOG_ERROR("Damaged header!");
        state = State::NoMsg;
        return;
    }

    LOG_DEBUG("Payload length: %lu", payloadLength);

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

        LOG_DEBUG("Payload length: %lu\n", payloadLength);
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
        state   = State::ReceivedPartialPayload;
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
        state = State::NoMsg;
        return;
    }

    state = State::ReceivedPayload;

    messageHandler->parseMessage(payload);

    if (!messageHandler->isValid() || messageHandler->isJSONNull()) {
        LOG_DEBUG("Error parsing JSON");
        state = State::NoMsg;
        return;
    }

    messageHandler->processMessage();
    state = State::NoMsg;
}

void StateMachine::setMessageHandler(std::unique_ptr<MessageHandler> handler)
{
    messageHandler = std::move(handler);
}
