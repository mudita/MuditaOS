#pragma once

#include <MessageType.hpp>
#include <Service/Message.hpp>

#include "../ServiceAntenna.hpp"

class AntennaLockRequestMessage : public sys::DataMessage
{
  public:
    AntennaLockRequestMessage(MessageType messageType, antenna::lockState req)
        : sys::DataMessage(messageType), request(req)
    {}
    AntennaLockRequestMessage(MessageType messageType) : sys::DataMessage(messageType)
    {}
    antenna::lockState request = antenna::lockState::unlocked;
};

class AntennaChangedMessage : public sys::DataMessage
{
  public:
    AntennaChangedMessage(void) : sys::DataMessage(MessageType::AntennaChanged)
    {}
};

class AntennaLockRequestResponse : public sys::ResponseMessage
{
  public:
    AntennaLockRequestResponse(bool retCode,
                               antenna::lockState response,
                               MessageType responseTo = MessageType::MessageTypeUninitialized)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode), data(response){};

    bool retCode;
    antenna::lockState data;
};
