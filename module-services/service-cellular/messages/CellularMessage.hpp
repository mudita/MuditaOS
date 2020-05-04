/*
 *  @file CellularMessage.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 03.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_CELLULARMESSAGE_HPP
#define PUREPHONE_CELLULARMESSAGE_HPP

#include "../SignalStrength.hpp"
#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <utf8/UTF8.hpp>
#include <memory>
#include "../State.hpp"
#include <Modem/TS0710/TS0710.h>

#include <PhoneNumber.hpp>

#include <string>

class CellularMessage : public sys::DataMessage
{
  public:
    CellularMessage(MessageType messageType) : sys::DataMessage(messageType){};
    virtual ~CellularMessage(){};
};

class CellularCallMessage : public CellularMessage
{
  public:
    enum class Type
    {
        Ringing,      // user provided number to call to and service initialized calling procedure.
        IncomingCall, // device receives connection from other device.
    };

    CellularCallMessage() = delete;
    CellularCallMessage(Type type, const utils::PhoneNumber::View &number)
        : CellularMessage(MessageType::CellularCall), type(type), number(number)
    {}
    CellularCallMessage(Type type, const utils::PhoneNumber &number)
        : CellularMessage(MessageType::CellularCall), type(type), number(number.makeView())
    {}
    CellularCallMessage(Type type, const std::string &e164number)
        : CellularMessage(MessageType::CellularCall), type(type), number(utils::PhoneNumber::viewFromE164(e164number))
    {}

    Type type;
    utils::PhoneNumber::View number;
};

class CellularNotificationMessage : public CellularMessage
{
  public:
    enum class Type
    {
        CallAborted, // user tried to call other device but receiving side dropped call or call unsuccessful
        CallActive,  // call is in progress both if call was initialized by user and when user received incoming call.
        NewIncomingSMS,           // device received new sms from network. (what about sms delivery reports?).
        SignalStrengthUpdate,     // update of the strength of the network's signal.
        PowerUpProcedureComplete, // modem without cmux on initialization complete (cold start || reset modem -> and
                                  // cold start)
        SIM,                      // change on SIM from URC
        RawCommand,               // send raw command to modem -> returns raw, tokenised result
    };

    // TODO check and fix all CellularNotificationMessage constructors

    CellularNotificationMessage() = delete;
    CellularNotificationMessage(Type type, const std::string &data = "")
        : CellularMessage(MessageType::CellularNotification), type(type), data(data)
    {}

    virtual ~CellularNotificationMessage() = default;

    Type type;
    std::string data;
};

class CellularRequestMessage : public CellularMessage
{
  public:
    CellularRequestMessage(MessageType messageType) : CellularMessage(messageType)
    {}
    ~CellularRequestMessage()
    {}

    std::string data;
};

class CellularCallRequestMessage : public CellularMessage
{
  public:
    CellularCallRequestMessage(const utils::PhoneNumber::View &number)
        : CellularMessage(MessageType::CellularCallRequest), number(number)
    {}
    utils::PhoneNumber::View number;
};
class CellularGetChannelMessage : public sys::DataMessage
{
  public:
    CellularGetChannelMessage(TS0710::Channel channel = TS0710::Channel::None)
        : sys::DataMessage(MessageType::CellularGetChannel), channel(channel)
    {}
    TS0710::Channel channel;
};
class CellularGetChannelResponseMessage : public sys::DataMessage
{
  public:
    CellularGetChannelResponseMessage(DLC_channel *channelPtr = nullptr)
        : sys::DataMessage(MessageType::CellularGetChannelResponse), channelPtr(channelPtr)
    {}
    DLC_channel *channelPtr;
};
class CellularGetChannelResponseMessage : public sys::DataMessage
{
  public:
    CellularGetChannelResponseMessage(DLC_channel *channelPtr = nullptr)
        : sys::DataMessage(MessageType::CellularGetChannelResponse), channelPtr(channelPtr)
    {}
    DLC_channel *channelPtr;
};

class CellularResponseMessage : public sys::ResponseMessage
{
  public:
    CellularResponseMessage(bool retCode,
                            std::string retdata    = std::string(),
                            MessageType responseTo = MessageType::MessageTypeUninitialized)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode), data(retdata){};
    CellularResponseMessage(bool retCode, MessageType responseTo)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode){};
    virtual ~CellularResponseMessage(){};

    bool retCode;
    std::string data;
};

namespace cellular
{

    class StateChange : public CellularMessage
    {
      public:
        const State::ST request;
        StateChange(const State::ST request) : CellularMessage(MessageType::CellularStateRequest), request(request)
        {}
    };

    class RawCommand : public CellularNotificationMessage
    {
      public:
        RawCommand() : CellularNotificationMessage(CellularNotificationMessage::Type::RawCommand){};
        virtual ~RawCommand() = default;
        std::string command;
        uint32_t timeout = 1000;
    };

    class RawCommandResp : public CellularResponseMessage
    {
      public:
        RawCommandResp(uint32_t retCode) : CellularResponseMessage(retCode){};
        virtual ~RawCommandResp() = default;
        std::vector<std::string> response;
    };

    class RawCommandRespAsync : public CellularMessage
    {
      public:
        RawCommandRespAsync(MessageType messageType) : CellularMessage(messageType){};
        virtual ~RawCommandRespAsync() = default;
        std::vector<std::string> data;
    };

} // namespace cellular

#endif // PUREPHONE_CELLULARMESSAGE_HPP
