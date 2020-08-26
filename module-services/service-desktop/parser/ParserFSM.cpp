#include "ParserFSM.hpp"
#include <service-desktop/ServiceDesktop.hpp>
#include <log/log.hpp>
#include <json/json11.hpp>
#include <memory>
#include <string>

using namespace parserFSM;

StateMachine::StateMachine(sys::Service *OwnerService) : OwnerServicePtr(OwnerService)
{}

void StateMachine::processMessage(std::string &msg)
{
    receivedMsgPtr = &msg;
    LOG_DEBUG("Msg: %s", receivedMsgPtr->c_str());

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

    auto messageStart = receivedMsgPtr->find(message::endpointChar);
    if (messageStart == std::string::npos) {
        LOG_ERROR("This is not a valid endpoint message! Type=%c\n", receivedMsgPtr->at(0));
        return;
    }

    if (receivedMsgPtr->size() < message::size_header) // header divided in few parts
    {
        state = State::ReceivedPartialHeader;
        header.append(*receivedMsgPtr); // append to whole header string
        return;
    }

    header        = message::getHeader(*receivedMsgPtr);
    payloadLength = message::calcPayloadLength(header);
    if (payloadLength == 0) // failed to obtain payload length from msg
    {
        LOG_ERROR("Damaged header!");
        state = State::NoMsg;
        return;
    }

    LOG_DEBUG("Payload length: %lu\n", payloadLength);

    message::removeHeader(*receivedMsgPtr);
    parseNewMessage();
}

void StateMachine::parsePartialHeader()
{
    auto previousHeaderLength = header.size();
    auto missingHeaderLength  = message::size_header - previousHeaderLength;

    if (receivedMsgPtr->size() >= missingHeaderLength) // rest of the message is here
    {
        header.append(receivedMsgPtr->substr(0, missingHeaderLength));
        LOG_DEBUG("Header: %s\n", header.c_str());
        payloadLength = message::calcPayloadLength(header);

        LOG_DEBUG("Payload length: %lu\n", payloadLength);
        message::eraseFront(*receivedMsgPtr, missingHeaderLength);

        parseNewMessage();
    }
    else // the message is even longer :(
    {
        header.append(*receivedMsgPtr);
    }
}

void StateMachine::parseNewMessage()
{
    if (receivedMsgPtr->size() >= payloadLength) {

        payload = message::extractPayload(*receivedMsgPtr, payloadLength);

        parsePayload();

        if (receivedMsgPtr->size() > payloadLength) { // contains part of new header
            message::eraseFront(*receivedMsgPtr, payloadLength);
            parseHeader();
        }
    }
    else // message divided in 2 or more packets
    {
        payload = receivedMsgPtr->substr(0, std::string::npos); // take rest of the message
        state   = State::ReceivedPartialPayload;
    }
}

void StateMachine::parsePartialMessage()
{
    auto previousPayloadLength = payload.size();
    auto missingPayloadLength  = payloadLength - previousPayloadLength;

    if (receivedMsgPtr->size() >= missingPayloadLength) // rest of the message is here
    {
        payload.append(message::extractPayload(*receivedMsgPtr, missingPayloadLength));

        parsePayload();

        if (receivedMsgPtr->size() > missingPayloadLength) {
            message::eraseFront(*receivedMsgPtr, missingPayloadLength);
            parseHeader();
        }
    }
    else // the message is even longer
    {
        payload.append(*receivedMsgPtr);
    }
}

void StateMachine::parsePayload()
{
    LOG_DEBUG("Payload: %s\n", payload.c_str());
    if (payload.empty()) {
        LOG_ERROR("Empty payload!\n");
        state = State::NoMsg;
        return;
    }

    state = State::ReceivedPayload;

    // processing payload
    auto handler = std::make_unique<MessageHandler>(payload, OwnerServicePtr);

    if (!handler->isValid() || handler->isJSONNull()) {
        LOG_DEBUG("JsonErr: %s", handler->getErrorString().c_str());
        state = State::NoMsg;
        return;
    }

    handler->processMessage();
}
