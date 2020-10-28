// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

#include "module-bsp/bsp/cellular/bsp_cellular.hpp"

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
        : CellularMessage(MessageType::CellularCall), type(type), number(number.getView())
    {}
    CellularCallMessage(Type type, const std::string &e164number)
        : CellularMessage(MessageType::CellularCall), type(type), number(utils::PhoneNumber::parse(e164number))
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
        NetworkStatusUpdate,      // update of the status of the network
        PowerUpProcedureComplete, // modem without cmux on initialization complete (cold start || reset modem -> and
                                  // cold start)
        SIM,                      // change on SIM from URC
        RawCommand,               // send raw command to modem -> returns raw, tokenised result
        PowerDownDeregistering,   // modem informed it has started to disconnect from network
        PowerDownDeregistered,    // modem informed it has disconnected from network
        NewIncomingUSSD,          // modem received new ussd code from network
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
class CellularTimeNotificationMessage : public CellularMessage
{
  private:
    struct tm time;

  public:
    CellularTimeNotificationMessage() = delete;
    explicit CellularTimeNotificationMessage(struct tm time)
        : CellularMessage(MessageType::CellularTimeUpdated), time(time)
    {}
    struct tm getTime(void)
    {
        return time;
    }
};
class CellularUSSDMessage : public CellularMessage
{
  public:
    enum class RequestType
    {
        pullSesionRequest,
        pushSesionRequest,
        abortSesion
    };
    CellularUSSDMessage() = delete;
    CellularUSSDMessage(RequestType requestType, const std::string &data = "")
        : CellularMessage(MessageType::CellularUSSDRequest), type(requestType), data(data)
    {}

    RequestType type;
    std::string data;
};
class CellularRequestMessage : public CellularMessage
{
  public:
    CellularRequestMessage(MessageType messageType, std::string data = "") : CellularMessage(messageType), data(data)
    {}
    ~CellularRequestMessage()
    {}

    std::string data;
};

class CellularDtmfRequestMessage : public CellularMessage
{
    uint32_t digit = 0;

  public:
    CellularDtmfRequestMessage(uint32_t digit) : CellularMessage(MessageType::CellularTransmitDtmfTones), digit(digit)
    {}

    uint32_t getDigit() const
    {
        return digit;
    }
};

class CellularAntennaRequestMessage : public CellularMessage
{
  public:
    CellularAntennaRequestMessage(MessageType messageType) : CellularMessage(messageType)
    {}

    bsp::cellular::antenna antenna;
};

class CellularCallRequestMessage : public CellularMessage
{
  public:
    CellularCallRequestMessage(const utils::PhoneNumber::View &number)
        : CellularMessage(MessageType::CellularCallRequest), number(number)
    {}
    utils::PhoneNumber::View number;
};

class CellularSimMessage : public CellularMessage
{
  public:
    enum class SimCard
    {
        SIM1,
        SIM2
    };

    CellularSimMessage(SimCard sim) : CellularMessage(MessageType::CellularSimProcedure), sim(sim)
    {}
    virtual ~CellularSimMessage() = default;
    SimCard getSimCard() const noexcept
    {
        return sim;
    }

  private:
    SimCard sim                         = defaultSimCard;
    static const SimCard defaultSimCard = SimCard::SIM1;
};

class CellularSimResponseMessage : public CellularSimMessage
{
  public:
    enum class SimState
    {
        SIMUnlocked,
        PINRequired,
        PINInvalidRetryPossible,
        PUKRequired,
        PUKInvalidRetryPossible,
        SIMBlocked
    };
    CellularSimResponseMessage(SimCard sim,
                               SimState state,
                               const utils::PhoneNumber::View &number,
                               unsigned int attemptsLeft = defaultAttemptsLeft)
        : CellularSimMessage(sim), state(state), number(number), attemptsLeft(attemptsLeft)
    {}

    SimState getSimState() const noexcept
    {
        return state;
    }
    utils::PhoneNumber::View getPhoneNumber() const noexcept
    {
        return number;
    }
    unsigned int getAttemptsLeft() const noexcept
    {
        return attemptsLeft;
    }

  private:
    SimState state = defaultSimState;
    utils::PhoneNumber::View number;
    /// ignored if state is not one of { PINInvalidRetryPossible, PUKInvalidRetryPossible }
    unsigned int attemptsLeft = defaultAttemptsLeft;

    static const unsigned int defaultAttemptsLeft = 4;
    static const SimState defaultSimState         = SimState::SIMUnlocked;
};

class CellularSimVerifyPinRequestMessage : public CellularSimMessage
{
  public:
    CellularSimVerifyPinRequestMessage(SimCard sim, std::vector<unsigned int> pinValue)
        : CellularSimMessage(sim), pinValue(std::move(pinValue))
    {}

    std::vector<unsigned int> gePinValue() const noexcept
    {
        return pinValue;
    }

  private:
    std::vector<unsigned int> pinValue;
};

class CellularGetChannelMessage : public sys::DataMessage
{
  public:
    CellularGetChannelMessage(TS0710::Channel dataChannel = TS0710::Channel::None)
        : sys::DataMessage(MessageType::CellularGetChannel), dataChannel(dataChannel)
    {}
    TS0710::Channel dataChannel;
};

class CellularGetChannelResponseMessage : public sys::DataMessage
{
  public:
    CellularGetChannelResponseMessage(DLC_channel *dataChannelPtr = nullptr)
        : sys::DataMessage(MessageType::CellularGetChannelResponse), dataChannelPtr(dataChannelPtr)
    {}
    DLC_channel *dataChannelPtr;
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

class CellularAntennaResponseMessage : public sys::ResponseMessage
{
  public:
    CellularAntennaResponseMessage(bool retCode, bsp::cellular::antenna retAntenna, MessageType responseTo)
        : sys::ResponseMessage(sys::ReturnCodes::Success, responseTo), retCode(retCode)
    {
        antenna = retAntenna;
    };

    bool retCode;
    bsp::cellular::antenna antenna;
};
namespace cellular
{

    class StateChange : public CellularMessage
    {
      public:
        const State::ST request;
        StateChange(const State::ST request = State::ST::Failed)
            : CellularMessage(MessageType::CellularStateRequest), request(request)
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
