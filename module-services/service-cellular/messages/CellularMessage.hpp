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
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "utf8/UTF8.hpp"
#include <memory>
#include <variant>

class CellularMessage : public sys::DataMessage
{
  public:
    CellularMessage(MessageType messageType) : sys::DataMessage(messageType){};
    virtual ~CellularMessage(){};
};

class CellularNotificationMessage : public CellularMessage
{
  public:
    enum class Type
    {
        IncomingCall, // device receives connection from other device.
        CallAborted,  // user tried to call other device but receiving side dropped call or call unsuccessful
        CallActive,   // call is in progress both if call was initialized by user and when user received incoming call.
        Ringing,      // user provided number to call to and service initialized calling procedure.
        NewIncomingSMS,       // device received new sms from network. (what about sms delivery reports?).
        SignalStrengthUpdate, // update of the strength of the network's signal.
        ServiceReady, // Idle state of the service. This is a start state before any call is initialized by user or by
                      // network. service returns to this state when call is finished.
        PowerUpProcedureComplete, // modem without cmux on initialization complete (cold start || reset modem -> and
                                  // cold start)
        SIM,                      // change on SIM from URC
        SanityCheck,
        ModemOn,           // Modem initialized
        ModemFatalFailure, // Modem messed up beyond recognition, needs reboot (ie. QSIMSEL not set, or just returns
                           // `OK` without message body (which happens sometimes...), or stopped totally responding
        RawCommand,        // send raw command to modem -> returns raw, tokenised result
        None
    };

    // TODO check and fix all CellularNotificationMessage constructors

    CellularNotificationMessage() = delete;
    CellularNotificationMessage(Type type, const std::string &data = "")
        : CellularMessage(MessageType::CellularNotification), type(type), data(data)
    {}

    virtual ~CellularNotificationMessage() = default;

    Type type = Type::None;

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
