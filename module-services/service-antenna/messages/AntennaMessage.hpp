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
    ~AntennaLockRequestMessage()
    {}

    antenna::lockState request = antenna::lockState::unlocked;
};

class AntennaLockRequestResponse : public sys::ResponseMessage
{
  public:
    AntennaLockRequestResponse(bool retCode,
                               antenna::lockState response,
                               MessageType responseTo = MessageType::MessageTypeUninitialized)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode), data(response){};
    virtual ~AntennaLockRequestResponse(){};

    bool retCode;
    antenna::lockState data;
};
