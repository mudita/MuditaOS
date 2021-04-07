// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SignalStrength.hpp"
#include "State.hpp"

#include <MessageType.hpp>
#include <Modem/TS0710/TS0710.h>
#include <PhoneNumber.hpp>
#include <Service/Message.hpp>
#include <module-bsp/bsp/cellular/bsp_cellular.hpp>
#include <utf8/UTF8.hpp>
#include <SimState.hpp>

#include <response.hpp>

#include <memory>
#include <string>

#include <service-appmgr/service-appmgr/messages/ActionRequest.hpp>
#include <service-appmgr/service-appmgr/Actions.hpp>
#include <service-appmgr/service-appmgr/data/SimActionsParams.hpp>
#include <service-appmgr/service-appmgr/data/MmiActionsParams.hpp>

class CellularMessage : public sys::DataMessage
{
  public:
    explicit CellularMessage(MessageType messageType) : sys::DataMessage(messageType){};
};

class CellularRingingMessage : public CellularMessage
{
  public:
    CellularRingingMessage(const utils::PhoneNumber::View &number)
        : CellularMessage(MessageType::CellularRinging), number(number)
    {}
    CellularRingingMessage(const utils::PhoneNumber &number)
        : CellularMessage(MessageType::CellularRinging), number(number.getView())
    {}
    CellularRingingMessage(const std::string &e164number)
        : CellularMessage(MessageType::CellularRinging), number(utils::PhoneNumber::parse(e164number))
    {}

    utils::PhoneNumber::View number;
};

class CellularIncominCallMessage : public CellularMessage
{
  public:
    CellularIncominCallMessage(const utils::PhoneNumber::View &number)
        : CellularMessage(MessageType::CellularIncomingCall), number(number)
    {}
    CellularIncominCallMessage(const utils::PhoneNumber &number)
        : CellularMessage(MessageType::CellularIncomingCall), number(number.getView())
    {}
    CellularIncominCallMessage(const std::string &e164number)
        : CellularMessage(MessageType::CellularIncomingCall), number(utils::PhoneNumber::parse(e164number))
    {}

    utils::PhoneNumber::View number;
};

class CellularCallerIdMessage : public CellularMessage
{
  public:
    CellularCallerIdMessage(const utils::PhoneNumber::View &number)
        : CellularMessage(MessageType::CellularCallerId), number(number)
    {}
    CellularCallerIdMessage(const utils::PhoneNumber &number)
        : CellularMessage(MessageType::CellularCallerId), number(number.getView())
    {}
    CellularCallerIdMessage(const std::string &e164number)
        : CellularMessage(MessageType::CellularCallerId), number(utils::PhoneNumber::parse(e164number))
    {}

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
        SIM_READY,                // change on SIM from URC
        SIM_NOT_READY,            // change to not existing/not valid SIM
        PowerDownDeregistering,   // modem informed it has started to disconnect from network
        PowerDownDeregistered,    // modem informed it has disconnected from network
        SMSDone,                  // SMS initialization finished
        NewIncomingUrc,           // phone received new URC from network and we need to wake up modem and host
        Ring,                     // phone received Ring notification
        CallerID                  // phone received Caller Id notification
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

class CellularGetCurrentOperatorMessage : public CellularMessage
{
  public:
    explicit CellularGetCurrentOperatorMessage() : CellularMessage(MessageType::CellularNotification)
    {}
};

class CellularSetOperatorAutoSelectMessage : public sys::DataMessage
{};

class CellularGetCurrentOperatorResponse : public CellularMessage
{
    std::string currentOperatorName;

  public:
    explicit CellularGetCurrentOperatorResponse(std::string currentOperatorName)
        : CellularMessage(MessageType::CellularNotification), currentOperatorName(currentOperatorName)
    {}

    std::string getCurrentOperatorName() const
    {
        return currentOperatorName;
    }
};
class CellularSetOperatorMessage : public sys::DataMessage
{
    at::response::cops::CopsMode mode;
    at::response::cops::NameFormat format;
    const std::string name;

  public:
    explicit CellularSetOperatorMessage(at::response::cops::CopsMode mode,
                                        at::response::cops::NameFormat format,
                                        std::string name)
        : mode(mode), format(format), name(std::move(name))
    {}

    at::response::cops::CopsMode getMode() const noexcept
    {
        return mode;
    }
    at::response::cops::NameFormat getFormat() const noexcept
    {
        return format;
    }

    std::string getName() const
    {
        return name;
    }
};

class CellularPowerStateChange : public CellularMessage
{
  public:
    explicit CellularPowerStateChange(cellular::State::PowerState new_state)
        : CellularMessage(MessageType::CellularPowerStateChange), newState(new_state)
    {}

    cellular::State::PowerState getNewState() const noexcept
    {
        return newState;
    }

  private:
    cellular::State::PowerState newState;
};

class CellularStartOperatorsScanMessage : public CellularMessage
{
    bool fullInfo = false;

  public:
    explicit CellularStartOperatorsScanMessage(bool fullInfoList = false)
        : CellularMessage(MessageType::CellularStartOperatorsScan), fullInfo(fullInfoList)
    {}

    bool getFullInfo() const noexcept
    {
        return fullInfo;
    }
};

class CellularSimStateMessage : public CellularMessage
{
  private:
    at::SimState state;
    std::string message;

  public:
    explicit CellularSimStateMessage(at::SimState state, std::string message)
        : CellularMessage(MessageType::CellularSimState), state(state), message(std::move(message))
    {}

    at::SimState getState() const noexcept
    {
        return state;
    }

    std::string getMessage() const
    {
        return message;
    }
};

class CellularTimeNotificationMessage : public CellularMessage
{
  private:
    std::optional<struct tm> time;
    std::optional<int> timeZoneGmtOff;
    std::optional<std::string> timeZoneString;

  public:
    CellularTimeNotificationMessage() = delete;
    explicit CellularTimeNotificationMessage(struct tm time, long int timeZoneGmtOff, std::string timeZoneString)
        : CellularMessage(MessageType::CellularTimeUpdated), time(time), timeZoneGmtOff(timeZoneGmtOff),
          timeZoneString(timeZoneString)
    {}

    explicit CellularTimeNotificationMessage(long int timeZoneGmtOff, std::string timeZoneString)
        : CellularMessage(MessageType::CellularTimeUpdated), timeZoneGmtOff(timeZoneGmtOff),
          timeZoneString(timeZoneString)
    {}

    explicit CellularTimeNotificationMessage(struct tm time)
        : CellularMessage(MessageType::CellularTimeUpdated), time(time)
    {}

    std::optional<struct tm> getTime(void)
    {
        return time;
    }

    std::optional<long int> getTimeZoneOffset(void)
    {
        return timeZoneGmtOff;
    }

    std::optional<const std::string> getTimeZoneString(void)
    {
        return timeZoneString;
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
    CellularAntennaRequestMessage() : CellularMessage(MessageType::CellularSelectAntenna)
    {}

    bsp::cellular::antenna antenna;
};

class CellularCallRequestMessage : public CellularMessage
{
  public:
    enum class RequestMode
    {
        Normal,
        Emergency
    };

    CellularCallRequestMessage(const utils::PhoneNumber::View &number, RequestMode requestMode = RequestMode::Normal)
        : CellularMessage(MessageType::CellularCallRequest), number(number), requestMode(requestMode)
    {}
    utils::PhoneNumber::View number;
    RequestMode requestMode = RequestMode::Normal;
};

class CellularSmsNoSimRequestMessage : public CellularMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    CellularSmsNoSimRequestMessage() : CellularMessage{MessageType::MessageTypeUninitialized}
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::SmsRejectNoSim, std::make_unique<app::manager::actions::ActionParams>());
    }
};

class CellularSimMessage : public CellularMessage
{
  public:
    CellularSimMessage(MessageType messageType, Store::GSM::SIM sim) : CellularMessage(messageType), sim(sim)
    {}
    virtual ~CellularSimMessage() = default;
    Store::GSM::SIM getSimCard() const noexcept
    {
        return sim;
    }

  private:
    Store::GSM::SIM sim                         = defaultSimCard;
    static const Store::GSM::SIM defaultSimCard = Store::GSM::SIM::SIM1;
};

/// Message use only for mockup GUI purposes
class CellularSimVerifyPinRequestMessage : public CellularSimMessage
{
  public:
    CellularSimVerifyPinRequestMessage(Store::GSM::SIM sim, std::vector<unsigned int> pinValue)
        : CellularSimMessage(MessageType::CellularSimVerifyPinRequest, sim), pinValue(std::move(pinValue))
    {}
    CellularSimVerifyPinRequestMessage(Store::GSM::SIM sim,
                                       std::vector<unsigned int> pinValue,
                                       std::vector<unsigned int> pukValue)
        : CellularSimMessage(MessageType::CellularSimVerifyPinRequest, sim), pinValue(std::move(pinValue)),
          pukValue(std::move(pukValue))
    {}

    std::vector<unsigned int> getPinValue() const
    {
        return pinValue;
    }
    std::vector<unsigned int> getPukValue() const
    {
        return pukValue;
    }

  private:
    std::vector<unsigned int> pinValue;
    std::vector<unsigned int> pukValue;
};

class CellularSimPasscodeRequest : public CellularMessage
{
  protected:
    app::manager::actions::PasscodeParams params;

    CellularSimPasscodeRequest(Store::GSM::SIM _sim, unsigned int _attempts, std::string _passcodeName)
        : CellularMessage(MessageType::CellularSimResponse), params(_sim, _attempts, std::move(_passcodeName))
    {}
};

class CellularSimRequestPinMessage : public CellularSimPasscodeRequest,
                                     public app::manager::actions::ConvertibleToAction
{
  public:
    CellularSimRequestPinMessage(Store::GSM::SIM _sim, unsigned int _attempts, std::string _passcodeName)
        : CellularSimPasscodeRequest(_sim, _attempts, std::move(_passcodeName))
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::RequestPin, std::make_unique<app::manager::actions::PasscodeParams>(params));
    }
};

class CellularSimRequestPukMessage : public CellularSimPasscodeRequest,
                                     public app::manager::actions::ConvertibleToAction
{
  public:
    CellularSimRequestPukMessage(Store::GSM::SIM _sim, unsigned int _attempts, std::string _passcodeName)
        : CellularSimPasscodeRequest(_sim, _attempts, std::move(_passcodeName))
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::RequestPuk, std::make_unique<app::manager::actions::PasscodeParams>(params));
    }
};

class CellularUnlockSimMessage : public CellularMessage, public app::manager::actions::ConvertibleToAction
{
    app::manager::actions::SimStateParams params;

  public:
    CellularUnlockSimMessage(Store::GSM::SIM _sim) : CellularMessage(MessageType::CellularSimResponse), params(_sim)
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::UnlockSim, std::make_unique<app::manager::actions::SimStateParams>(params));
    }
};

class CellularBlockSimMessage : public CellularMessage, public app::manager::actions::ConvertibleToAction
{
    app::manager::actions::SimStateParams params;

  public:
    explicit CellularBlockSimMessage(Store::GSM::SIM _sim)
        : CellularMessage(MessageType::CellularSimResponse), params(_sim)
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::BlockSim, std::make_unique<app::manager::actions::SimStateParams>(params));
    }
};

class CellularDisplayCMEMessage : public CellularMessage, public app::manager::actions::ConvertibleToAction
{
    app::manager::actions::UnhandledCMEParams params;

  public:
    CellularDisplayCMEMessage(Store::GSM::SIM _sim, unsigned int _cmeCode)
        : CellularMessage(MessageType::CellularSimResponse), params(_sim, _cmeCode)
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender,
            app::manager::actions::DisplayCMEError,
            std::make_unique<app::manager::actions::UnhandledCMEParams>(params));
    }
};

class CellularSimDataMessage : public CellularMessage
{
    Store::GSM::SIM sim = Store::GSM::SIM::NONE;

  public:
    explicit CellularSimDataMessage(Store::GSM::SIM _sim) : CellularMessage{MessageType::CellularSimResponse}, sim{_sim}
    {}
    [[nodiscard]] Store::GSM::SIM getSim() const noexcept
    {
        return sim;
    }
};

class CellularChangeSimDataMessage : public CellularSimDataMessage
{
  public:
    CellularChangeSimDataMessage(Store::GSM::SIM _sim) : CellularSimDataMessage{_sim}
    {}
};

class CellularSimPinDataMessage : public CellularSimDataMessage
{
    std::vector<unsigned int> pinValue;

  public:
    CellularSimPinDataMessage(Store::GSM::SIM _sim, std::vector<unsigned int> _pinValue)
        : CellularSimDataMessage{_sim}, pinValue{std::move(_pinValue)}
    {}

    [[nodiscard]] const std::vector<unsigned int> &getPin() const noexcept
    {
        return pinValue;
    }
};

class CellularSimNewPinDataMessage : public CellularSimDataMessage
{
    std::vector<unsigned int> oldPin;
    std::vector<unsigned int> newPin;

  public:
    CellularSimNewPinDataMessage(Store::GSM::SIM _sim,
                                 std::vector<unsigned int> _oldPin,
                                 std::vector<unsigned int> _newPin)
        : CellularSimDataMessage{_sim}, oldPin{std::move(_oldPin)}, newPin{std::move(_newPin)}
    {}

    [[nodiscard]] const std::vector<unsigned int> &getOldPin() const noexcept
    {
        return oldPin;
    }
    [[nodiscard]] const std::vector<unsigned int> &getNewPin() const noexcept
    {
        return newPin;
    }
};

class CellularSimCardPinLockStateRequestDataMessage : public sys::DataMessage
{};

class CellularSimCardPinLockStateResponseDataMessage : public sys::DataMessage
{
  public:
    explicit CellularSimCardPinLockStateResponseDataMessage(bool state) : simCardPinLockState(state)
    {}

    [[nodiscard]] bool getSimCardPinLockState() const noexcept
    {
        return simCardPinLockState;
    }

  private:
    const bool simCardPinLockState;
};

class CellularSimCardLockDataMessage : public CellularSimDataMessage
{

  public:
    enum class SimCardLock
    {
        Locked,
        Unlocked
    };
    CellularSimCardLockDataMessage(Store::GSM::SIM _sim, SimCardLock _simCardLock, std::vector<unsigned int> _pin)
        : CellularSimDataMessage{_sim}, simCardLock{_simCardLock}, pin{std::move(_pin)}
    {}

    [[nodiscard]] SimCardLock getLock() const noexcept
    {
        return simCardLock;
    }
    [[nodiscard]] const std::vector<unsigned int> &getPin() const noexcept
    {
        return pin;
    }

  private:
    SimCardLock simCardLock;
    std::vector<unsigned int> pin;
};

class CellularSimPukDataMessage : public CellularSimDataMessage
{
    std::vector<unsigned int> puk;
    std::vector<unsigned int> newPin;

  public:
    CellularSimPukDataMessage(Store::GSM::SIM _sim, std::vector<unsigned int> _puk, std::vector<unsigned int> _newPin)
        : CellularSimDataMessage{_sim}, puk{std::move(_puk)}, newPin{std::move(_newPin)}
    {}

    [[nodiscard]] const std::vector<unsigned int> &getPuk() const noexcept
    {
        return puk;
    }
    [[nodiscard]] const std::vector<unsigned int> &getNewPin() const noexcept
    {
        return newPin;
    }
};

class CellularSimAbortMessage : public CellularSimDataMessage
{
  public:
    explicit CellularSimAbortMessage(Store::GSM::SIM _sim) : CellularSimDataMessage{_sim}
    {}
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

class CellularSimNewPinResponseMessage : public CellularResponseMessage
{
  public:
    explicit CellularSimNewPinResponseMessage(bool retCode) : CellularResponseMessage(retCode)
    {}
};

class CellularSimPukResponseMessage : public CellularResponseMessage
{
  public:
    explicit CellularSimPukResponseMessage(bool retCode) : CellularResponseMessage(retCode)
    {}
};

class CellularSimCardLockResponseMessage : public CellularResponseMessage
{
    CellularSimCardLockDataMessage::SimCardLock cardLock;

  public:
    CellularSimCardLockResponseMessage(bool retCode, CellularSimCardLockDataMessage::SimCardLock cardLock)
        : CellularResponseMessage(retCode), cardLock(cardLock)
    {}

    [[nodiscard]] auto getSimCardLock() const noexcept -> CellularSimCardLockDataMessage::SimCardLock
    {
        return cardLock;
    }
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

class CellularMMIResult : public CellularMessage
{
  protected:
    app::manager::actions::MMIResultParams params;

    explicit CellularMMIResult(app::manager::actions::MMIResultParams::MMIResult result,
                               std::shared_ptr<app::manager::actions::MMICustomResultParams> customResult = nullptr)
        : CellularMessage(MessageType::CellularMMIData), params(result, std::move(customResult))
    {}
};

class CellularMMIResultMessage : public CellularMMIResult, public app::manager::actions::ConvertibleToAction
{
  public:
    explicit CellularMMIResultMessage(
        app::manager::actions::MMIResultParams::MMIResult result,
        std::shared_ptr<app::manager::actions::MMICustomResultParams> customResult = nullptr)
        : CellularMMIResult(result, std::move(customResult))
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender,
            app::manager::actions::ShowMMIResult,
            std::make_unique<app::manager::actions::MMIResultParams>(params));
    }
};
class CellularMMIDataMessage : public CellularMessage
{
  protected:
    app::manager::actions::MMIParams params;

  public:
    explicit CellularMMIDataMessage(std::string mmiData)
        : CellularMessage(MessageType::CellularMMIData), params(mmiData)
    {}
};
class CellularMMIResponseMessage : public CellularMMIDataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    explicit CellularMMIResponseMessage(std::string mmiData) : CellularMMIDataMessage(std::move(mmiData))
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::ShowMMIResponse, std::make_unique<app::manager::actions::MMIParams>(params));
    }
};
class CellularMMIPushMessage : public CellularMMIDataMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    explicit CellularMMIPushMessage(std::string mmiData) : CellularMMIDataMessage(std::move(mmiData))
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender, app::manager::actions::ShowMMIPush, std::make_unique<app::manager::actions::MMIParams>(params));
    }
};

class CellularSetOperatorAutoSelectResponse : public CellularResponseMessage
{
  public:
    explicit CellularSetOperatorAutoSelectResponse(bool ret) : CellularResponseMessage(ret)
    {}
};

class CellularSetOperatorResponse : public CellularResponseMessage
{
  public:
    explicit CellularSetOperatorResponse(bool ret) : CellularResponseMessage(ret)
    {}
};

class CellularVoLTEDataMessage : public CellularMessage
{
    bool VoLTEon = false;

  public:
    explicit CellularVoLTEDataMessage(bool VoLTEon) : CellularMessage{MessageType::CellularSetVoLTE}, VoLTEon{VoLTEon}
    {}
    [[nodiscard]] bool getVoLTEon() const noexcept
    {
        return VoLTEon;
    }
};

class CellularChangeVoLTEDataMessage : public CellularVoLTEDataMessage
{
  public:
    explicit CellularChangeVoLTEDataMessage(bool VoLTEon) : CellularVoLTEDataMessage{VoLTEon}
    {}
};

class CellularCheckIfStartAllowedMessage : public sys::DataMessage
{};

class CellularNoSimNotification : public CellularResponseMessage, public app::manager::actions::ConvertibleToAction
{
  public:
    CellularNoSimNotification(std::string data) : CellularResponseMessage(false, data)
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender,
            app::manager::actions::NoSimNotification,
            std::make_unique<app::manager::actions::ActionParams>(data));
    }
};

class CellularNotAnEmergencyNotification : public CellularResponseMessage,
                                           public app::manager::actions::ConvertibleToAction
{
  public:
    CellularNotAnEmergencyNotification(std::string data) : CellularResponseMessage(false, data)
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(
            sender,
            app::manager::actions::NotAnEmergencyNotification,
            std::make_unique<app::manager::actions::ActionParams>(data));
    }
};

class CellularNewIncomingSMSMessage : public CellularMessage
{
  public:
    explicit CellularNewIncomingSMSMessage(const std::string &data)
        : CellularMessage(MessageType::CellularNewIncomingSMS), notificationData(data)
    {}
    auto getData() const -> std::string
    {
        return notificationData;
    }

  private:
    std::string notificationData;
};

class CellularAnswerIncomingCallMessage : public CellularMessage
{
  public:
    CellularAnswerIncomingCallMessage() : CellularMessage(MessageType::CellularAnswerIncomingCall)
    {}
};

class CellularHangupCallMessage : public CellularMessage
{
  public:
    CellularHangupCallMessage() : CellularMessage(MessageType::CellularHangupCall)
    {}
};

class CellularListCallsMessage : public CellularMessage
{
  public:
    CellularListCallsMessage() : CellularMessage(MessageType::CellularListCurrentCalls)
    {}
};

class CellularSimProcedureMessage : public CellularMessage
{
  public:
    CellularSimProcedureMessage() : CellularMessage(MessageType::CellularSimProcedure)
    {}
};

class CellularGetIMSIMessage : public CellularMessage
{
  public:
    CellularGetIMSIMessage() : CellularMessage(MessageType::CellularGetIMSI)
    {}
};

class CellularGetOwnNumberMessage : public CellularMessage
{
  public:
    CellularGetOwnNumberMessage() : CellularMessage(MessageType::CellularGetOwnNumber)
    {}
};

class CellularGetNetworkInfoMessage : public CellularMessage
{
  public:
    CellularGetNetworkInfoMessage() : CellularMessage(MessageType::CellularGetNetworkInfo)
    {}
};

class CellularSetScanModeMessage : public CellularMessage
{
  public:
    CellularSetScanModeMessage(const std::string &mode) : CellularMessage(MessageType::CellularSetScanMode), data(mode)
    {}
    std::string data;
};

class CellularGetScanModeMessage : public CellularMessage
{
  public:
    CellularGetScanModeMessage() : CellularMessage(MessageType::CellularGetScanMode)
    {}
};

class CellularGetFirmwareVersionMessage : public CellularMessage
{
  public:
    CellularGetFirmwareVersionMessage() : CellularMessage(MessageType::CellularGetFirmwareVersion)
    {}
};

class CellularGetCsqMessage : public CellularMessage
{
  public:
    CellularGetCsqMessage() : CellularMessage(MessageType::CellularGetCSQ)
    {}
};

class CellularGetCregMessage : public CellularMessage
{
  public:
    CellularGetCregMessage() : CellularMessage(MessageType::CellularGetCREG)
    {}
};

class CellularGetNwinfoMessage : public CellularMessage
{
  public:
    CellularGetNwinfoMessage() : CellularMessage(MessageType::CellularGetNWINFO)
    {}
};

class CellularGetAntennaMessage : public CellularMessage
{
  public:
    CellularGetAntennaMessage() : CellularMessage(MessageType::CellularGetAntenna)
    {}
};

class CellularSetFlightModeMessage : public CellularMessage
{
  public:
    explicit CellularSetFlightModeMessage(bool flightModeOn)
        : CellularMessage(MessageType::CellularSetFlightMode), flightModeOn(flightModeOn)
    {}
    bool flightModeOn;
};

class CellularCallActiveNotification : public CellularNotificationMessage
{
  public:
    explicit CellularCallActiveNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::CallActive, data)
    {}
};

class CellularCallAbortedNotification : public CellularNotificationMessage
{
  public:
    explicit CellularCallAbortedNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::CallAborted, data)
    {}
};

class CellularPowerUpProcedureCompleteNotification : public CellularNotificationMessage
{
  public:
    explicit CellularPowerUpProcedureCompleteNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::PowerUpProcedureComplete, data)
    {}
};

class CellularPowerDownDeregisteringNotification : public CellularNotificationMessage
{
  public:
    explicit CellularPowerDownDeregisteringNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::PowerDownDeregistering, data)
    {}
};

class CellularPowerDownDeregisteredNotification : public CellularNotificationMessage
{
  public:
    explicit CellularPowerDownDeregisteredNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::PowerDownDeregistered, data)
    {}
};

class CellularNewIncomingSMSNotification : public CellularNotificationMessage
{
  public:
    explicit CellularNewIncomingSMSNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::NewIncomingSMS, data)
    {}
};

class CellularSimReadyNotification : public CellularNotificationMessage
{
  public:
    explicit CellularSimReadyNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::SIM_READY, data)
    {}
};

class CellularSmsDoneNotification : public CellularNotificationMessage
{
  public:
    explicit CellularSmsDoneNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::SMSDone, data)
    {}
};

class CellularSignalStrengthUpdateNotification : public CellularNotificationMessage
{
  public:
    explicit CellularSignalStrengthUpdateNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::SignalStrengthUpdate, data)
    {}
};

class CellularNetworkStatusUpdateNotification : public CellularNotificationMessage
{
  public:
    explicit CellularNetworkStatusUpdateNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::NetworkStatusUpdate, data)
    {}
};

class CellularSimNotReadyNotification : public CellularNotificationMessage
{
  public:
    explicit CellularSimNotReadyNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::SIM_NOT_READY, data)
    {}
};

class CellularUrcIncomingNotification : public CellularNotificationMessage
{
  public:
    explicit CellularUrcIncomingNotification(const std::string &data = "")
        : CellularNotificationMessage(CellularNotificationMessage::Type::NewIncomingUrc, data)
    {}
};

class CellularSetRadioOnOffMessage : public CellularMessage
{
  public:
    explicit CellularSetRadioOnOffMessage(bool radioOnOff)
        : CellularMessage(MessageType::CellularRadioOnOff), radioOnOff(radioOnOff)
    {}
    auto getRadioOnOf() -> bool
    {
        return radioOnOff;
    }

  private:
    bool radioOnOff;
};

class CellularSMSRejectedByOfflineNotification : public CellularResponseMessage,
                                                 public app::manager::actions::ConvertibleToAction
{
  public:
    CellularSMSRejectedByOfflineNotification() : CellularResponseMessage(false)
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(sender,
                                                             app::manager::actions::SMSRejectedByOfflineNotification,
                                                             std::make_unique<app::manager::actions::ActionParams>());
    }
};

class CellularCallRejectedByOfflineNotification : public CellularResponseMessage,
                                                  public app::manager::actions::ConvertibleToAction
{
  public:
    CellularCallRejectedByOfflineNotification() : CellularResponseMessage(false)
    {}

    [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
    {
        return std::make_unique<app::manager::ActionRequest>(sender,
                                                             app::manager::actions::CallRejectedByOfflineNotification,
                                                             std::make_unique<app::manager::actions::ActionParams>());
    }
};

class CellularSendSMSMessage : public CellularMessage
{
  public:
    explicit CellularSendSMSMessage(SMSRecord record) : CellularMessage(MessageType::CellularSendSMS), record(record)
    {}
    SMSRecord record;
};

class CellularRingNotification : public CellularNotificationMessage
{
  public:
    explicit CellularRingNotification(const utils::PhoneNumber::View &number)
        : CellularNotificationMessage(CellularNotificationMessage::Type::Ring), number(number)
    {}
    explicit CellularRingNotification(const utils::PhoneNumber &number)
        : CellularNotificationMessage(CellularNotificationMessage::Type::Ring), number(number.getView())
    {}
    explicit CellularRingNotification(const std::string &e164number)
        : CellularNotificationMessage(CellularNotificationMessage::Type::Ring),
          number(utils::PhoneNumber::parse(e164number))
    {}
    auto getNubmer() const -> utils::PhoneNumber::View
    {
        return number;
    }

  private:
    utils::PhoneNumber::View number;
};

class CellularCallerIdNotification : public CellularNotificationMessage
{
  public:
    explicit CellularCallerIdNotification(const utils::PhoneNumber::View &number)
        : CellularNotificationMessage(CellularNotificationMessage::Type::Ring), number(number)
    {}
    explicit CellularCallerIdNotification(const utils::PhoneNumber &number)
        : CellularNotificationMessage(CellularNotificationMessage::Type::Ring), number(number.getView())
    {}
    explicit CellularCallerIdNotification(const std::string &e164number)
        : CellularNotificationMessage(CellularNotificationMessage::Type::Ring),
          number(utils::PhoneNumber::parse(e164number))
    {}
    auto getNubmer() const -> utils::PhoneNumber::View
    {
        return number;
    }

  private:
    utils::PhoneNumber::View number;
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

    class RawCommandRespAsync : public CellularMessage
    {
      public:
        RawCommandRespAsync(MessageType messageType) : CellularMessage(messageType){};
        virtual ~RawCommandRespAsync() = default;
        std::vector<std::string> data;
    };

} // namespace cellular
