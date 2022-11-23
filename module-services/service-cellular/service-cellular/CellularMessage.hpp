// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SignalStrength.hpp"
#include <service-cellular/State.hpp>
#include <service-cellular/VolteState.hpp>

#include <modem/mux/CellularMux.h>
#include <PhoneNumber.hpp>
#include <Service/Message.hpp>
#include <module-bsp/bsp/cellular/bsp_cellular.hpp>
#include <utf8/UTF8.hpp>
#include <SimState.hpp>
#include <module-cellular/at/SimInsertedState.hpp>

#include <response.hpp>

#include <memory>
#include <string>

#include <service-appmgr/messages/ActionRequest.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <service-appmgr/data/CallActionsParams.hpp>

#include <service-cellular/api/common.hpp>
#include <DTMFCode.hpp>

class CellularMessage : public sys::DataMessage
{
  public:
    enum class Type
    {
        Uninitialized,
        StateRequest,       ///< cellular change state request, only for use by cellular
        Notification,       ///< Async notification message
        AnswerIncomingCall, ///< Answer incoming call
        HangupCall,         ///< Hang up call
        DismissCall,        ///< Dismiss incoming call request
        Ringing,
        IncomingCall,
        CallerId,
        CallRequest,      ///< Call request
        PowerStateChange, ///< Change power state of the module

        ListCurrentCalls,
        SimResponse, // Send to PIN window (show, error state, hide)
        SetFlightMode,

        PacketData, ///< for all PacketData messages

        GetOwnNumber,
        GetIMSI,
        GetNetworkInfo,
        StartOperatorsScan,
        OperatorsScanResult,
        NetworkInfoResult,
        SelectAntenna,
        SetScanMode,
        GetScanMode,
        GetScanModeResult,
        GetFirmwareVersion,       ///< Asks for current firmware version
        GetFirmwareVersionResult, ///< Returns current firmware version
        GetCSQ,
        GetCREG,
        GetNWINFO,
        GetAntenna,
        TransmitDtmfTones,
        USSDRequest,
        TimeUpdated,
        SimState,
        MMIData,
        NewIncomingSMS,
        IncomingSMSNotification,
        RadioOnOff,
        SendSMS,
        CellularSetConnectionFrequency
    };
    explicit CellularMessage(Type type) : sys::DataMessage(), type(type)
    {}
    const Type type;
};
namespace cellular
{

    class RingingMessage : public CellularMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        RingingMessage(const utils::PhoneNumber::View &number) : CellularMessage(Type::Ringing), number(number)
        {}
        RingingMessage(const utils::PhoneNumber &number) : CellularMessage(Type::Ringing), number(number.getView())
        {}
        RingingMessage(const std::string &e164number)
            : CellularMessage(Type::Ringing), number(utils::PhoneNumber::parse(e164number))
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::HandleOutgoingCall,
                std::make_unique<app::manager::actions::CallParams>(number));
        }

        utils::PhoneNumber::View number;
    };

    class NotificationMessage : public CellularMessage
    {
      public:
        enum class Content
        {
            CallAborted,    // user tried to call other device but receiving side dropped call or call unsuccessful
            CallActive,     // call is in progress both if call was initialized by user and when user received incoming
                            // call.
            NewIncomingSMS, // device received new sms from network. (what about sms delivery reports?).
            SignalStrengthUpdate,     // update of the strength of the network's signal.
            NetworkStatusUpdate,      // update of the status of the network
            PowerUpProcedureComplete, // modem without cmux on initialization complete (cold start || reset modem -> and
            // cold start)
            PowerDownDeregistering, // modem informed it has started to disconnect from network
            PowerDownDeregistered,  // modem informed it has disconnected from network
            SMSDone,                // SMS initialization finished
            NewIncomingUrc,         // phone received new URC from network and we need to wake up modem and host
            Ring,                   // phone received Ring notification
            CallerID                // phone received Caller Id notification
        };

        // TODO check and fix all NotificationMessage constructors

        NotificationMessage() = delete;
        NotificationMessage(Content content, const std::string &data = "")
            : CellularMessage(Type::Notification), content(content), data(data)
        {}

        virtual ~NotificationMessage() = default;

        Content content;
        std::string data;
    };

    class RequestCurrentOperatorNameMessage : public CellularMessage
    {
      public:
        explicit RequestCurrentOperatorNameMessage() : CellularMessage(Type::Notification)
        {}
    };

    class SetOperatorAutoSelectMessage : public sys::DataMessage
    {};

    class CurrentOperatorNameNotification : public CellularMessage
    {
        std::string currentOperatorName;

      public:
        explicit CurrentOperatorNameNotification(const std::string &currentOperatorName)
            : CellularMessage(Type::Notification), currentOperatorName(currentOperatorName)
        {}

        std::string getCurrentOperatorName() const
        {
            return currentOperatorName;
        }
    };
    class SetOperatorMessage : public sys::DataMessage
    {
        at::response::cops::CopsMode mode;
        at::response::cops::NameFormat format;
        const std::string name;

      public:
        explicit SetOperatorMessage(at::response::cops::CopsMode mode,
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

    class PowerStateChange : public CellularMessage
    {
      public:
        explicit PowerStateChange(cellular::service::State::PowerState new_state)
            : CellularMessage(Type::PowerStateChange), newState(new_state)
        {}

        cellular::service::State::PowerState getNewState() const noexcept
        {
            return newState;
        }

      private:
        cellular::service::State::PowerState newState;
    };

    class StartOperatorsScanMessage : public CellularMessage
    {
        bool fullInfo = false;

      public:
        explicit StartOperatorsScanMessage(bool fullInfoList = false)
            : CellularMessage(Type::StartOperatorsScan), fullInfo(fullInfoList)
        {}

        bool getFullInfo() const noexcept
        {
            return fullInfo;
        }
    };

    class TimeNotificationMessage : public CellularMessage
    {
      private:
        std::optional<struct tm> time;
        std::optional<int> timeZoneGmtOff;
        std::optional<std::string> timeZoneString;

      public:
        TimeNotificationMessage() = delete;
        explicit TimeNotificationMessage(struct tm time, long int timeZoneGmtOff, std::string timeZoneString)
            : CellularMessage(Type::TimeUpdated), time(time), timeZoneGmtOff(timeZoneGmtOff),
              timeZoneString(timeZoneString)
        {}

        explicit TimeNotificationMessage(long int timeZoneGmtOff, std::string timeZoneString)
            : CellularMessage(Type::TimeUpdated), timeZoneGmtOff(timeZoneGmtOff), timeZoneString(timeZoneString)
        {}

        explicit TimeNotificationMessage(struct tm time) : CellularMessage(Type::TimeUpdated), time(time)
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
    class USSDMessage : public CellularMessage
    {
      public:
        enum class RequestType
        {
            pullSessionRequest,
            pushSessionRequest,
            abortSession
        };
        USSDMessage() = delete;
        USSDMessage(RequestType requestType, const std::string &data = "")
            : CellularMessage(Type::USSDRequest), type(requestType), data(data)
        {}

        RequestType type;
        std::string data;
    };
    class RequestMessage : public CellularMessage
    {
      public:
        RequestMessage(Type type, std::string data = "") : CellularMessage(type), data(data)
        {}

        std::string data;
    };

    class DtmfRequestMessage : public CellularMessage
    {
        DTMFCode code;

      public:
        DtmfRequestMessage(DTMFCode code) : CellularMessage(Type::TransmitDtmfTones), code(code)
        {}

        DTMFCode getDTMFCode() const
        {
            return code;
        }
    };

    class AntennaRequestMessage : public CellularMessage
    {
      public:
        AntennaRequestMessage() : CellularMessage(Type::SelectAntenna)
        {}

        bsp::cellular::antenna antenna;
    };

    class CallRequestMessage : public CellularMessage
    {
      public:
        CallRequestMessage(const utils::PhoneNumber::View &number,
                           cellular::api::CallMode callMode = cellular::api::CallMode::Regular)
            : CellularMessage(Type::CallRequest), number(number), callMode(callMode)
        {}
        utils::PhoneNumber::View number;
        cellular::api::CallMode callMode = cellular::api::CallMode::Regular;
    };

    class SmsNoSimRequestMessage : public CellularMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        SmsNoSimRequestMessage() : CellularMessage{Type::Uninitialized}
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender, app::manager::actions::SmsRejectNoSim, std::make_unique<app::manager::actions::ActionParams>());
        }
    };

    class ResponseMessage : public sys::ResponseMessage
    {
      public:
        ResponseMessage(bool retCode,
                        std::string retdata              = std::string(),
                        CellularMessage::Type responseTo = CellularMessage::Type::Uninitialized)
            : sys::ResponseMessage(sys::ReturnCodes::Success), retCode(retCode), data(retdata),
              cellResponse(responseTo){};

        ResponseMessage(bool retCode, CellularMessage::Type responseTo)
            : sys::ResponseMessage(sys::ReturnCodes::Success), retCode(retCode), cellResponse(responseTo){};

        virtual ~ResponseMessage(){};

        bool retCode;
        std::string data;
        CellularMessage::Type cellResponse;
    };

    class GetOwnNumberResponseMessage : public ResponseMessage
    {
      public:
        GetOwnNumberResponseMessage(bool retCode, std::string number = std::string())
            : ResponseMessage(retCode, std::move(number))
        {}
    };

    class AntennaResponseMessage : public ResponseMessage
    {
      public:
        AntennaResponseMessage(bool retCode, bsp::cellular::antenna retAntenna, CellularMessage::Type responseTo)
            : ResponseMessage(retCode, responseTo), antenna(retAntenna)
        {}

        bsp::cellular::antenna antenna;
    };

    class MMIResult : public CellularMessage
    {
      protected:
        app::manager::actions::MMIResultParams params;

        explicit MMIResult(app::manager::actions::MMIResultParams::MMIResult result,
                           std::shared_ptr<app::manager::actions::MMICustomResultParams> customResult = nullptr)
            : CellularMessage(Type::MMIData), params(result, std::move(customResult))
        {}
    };

    class MMIResultMessage : public MMIResult, public app::manager::actions::ConvertibleToAction
    {
      public:
        explicit MMIResultMessage(app::manager::actions::MMIResultParams::MMIResult result,
                                  std::shared_ptr<app::manager::actions::MMICustomResultParams> customResult = nullptr)
            : MMIResult(result, std::move(customResult))
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::ShowMMIResult,
                std::make_unique<app::manager::actions::MMIResultParams>(params));
        }
    };
    class MMIDataMessage : public CellularMessage
    {
      protected:
        app::manager::actions::MMIParams params;

      public:
        explicit MMIDataMessage(std::string mmiData) : CellularMessage(Type::MMIData), params(mmiData)
        {}
    };
    class MMIResponseMessage : public MMIDataMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        explicit MMIResponseMessage(std::string mmiData) : MMIDataMessage(std::move(mmiData))
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::ShowMMIResponse,
                std::make_unique<app::manager::actions::MMIParams>(params));
        }
    };
    class MMIPushMessage : public MMIDataMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        explicit MMIPushMessage(std::string mmiData) : MMIDataMessage(std::move(mmiData))
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender, app::manager::actions::ShowMMIPush, std::make_unique<app::manager::actions::MMIParams>(params));
        }
    };

    class SetOperatorAutoSelectResponse : public ResponseMessage
    {
      public:
        explicit SetOperatorAutoSelectResponse(bool ret) : ResponseMessage(ret)
        {}
    };

    class SetOperatorResponse : public ResponseMessage
    {
      public:
        explicit SetOperatorResponse(bool ret) : ResponseMessage(ret)
        {}
    };

    class CheckIfStartAllowedMessage : public sys::DataMessage
    {};

    class NoSimNotification : public ResponseMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        NoSimNotification(std::string data) : ResponseMessage(false, data)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::NoSimNotification,
                std::make_unique<app::manager::actions::ActionParams>(data));
        }
    };

    class NotAnEmergencyNotification : public ResponseMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        NotAnEmergencyNotification(std::string data) : ResponseMessage(false, data)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::NotAnEmergencyNotification,
                std::make_unique<app::manager::actions::ActionParams>(data));
        }
    };

    class NoNetworkConenctionNotification : public ResponseMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        NoNetworkConenctionNotification() : ResponseMessage(false)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::NoNetworkConnectionNotification,
                std::make_unique<app::manager::actions::ActionParams>());
        }
    };

    class CallRequestGeneralError : public ResponseMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        using Error     = app::manager::actions::CallRequestGeneralErrorParams::Error;
        using ErrorType = app::manager::actions::CallRequestGeneralErrorParams::Error::Type;

        CallRequestGeneralError(ErrorType errorType) : ResponseMessage(false), error{errorType}
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::CallRequestGeneralErrorNotification,
                std::make_unique<app::manager::actions::CallRequestGeneralErrorParams>(error));
        }

      private:
        Error error;
    };

    class NewIncomingSMSMessage : public CellularMessage
    {
      public:
        explicit NewIncomingSMSMessage(const std::string &data)
            : CellularMessage(Type::NewIncomingSMS), notificationData(data)
        {}
        auto getData() const -> std::string
        {
            return notificationData;
        }

      private:
        std::string notificationData;
    };

    class IncomingSMSNotificationMessage : public CellularMessage
    {
      public:
        IncomingSMSNotificationMessage() : CellularMessage(Type::IncomingSMSNotification)
        {}
    };

    class AnswerIncomingCallMessage : public CellularMessage
    {
      public:
        AnswerIncomingCallMessage() : CellularMessage(Type::AnswerIncomingCall)
        {}
    };

    class HangupCallMessage : public CellularMessage
    {
      public:
        HangupCallMessage() : CellularMessage(Type::HangupCall)
        {}
    };

    class DismissCallMessage : public CellularMessage
    {
      public:
        DismissCallMessage() : CellularMessage(Type::DismissCall)
        {}
    };

    class ListCallsMessage : public CellularMessage
    {
      public:
        ListCallsMessage() : CellularMessage(Type::ListCurrentCalls)
        {}
    };

    class GetIMSIMessage : public CellularMessage
    {
      public:
        GetIMSIMessage() : CellularMessage(Type::GetIMSI)
        {}
    };

    class GetOwnNumberMessage : public CellularMessage
    {
      public:
        GetOwnNumberMessage() : CellularMessage(Type::GetOwnNumber)
        {}
    };

    class GetNetworkInfoMessage : public CellularMessage
    {
      public:
        GetNetworkInfoMessage() : CellularMessage(Type::GetNetworkInfo)
        {}
    };

    class SetScanModeMessage : public CellularMessage
    {
      public:
        SetScanModeMessage(const std::string &mode) : CellularMessage(Type::SetScanMode), data(mode)
        {}
        std::string data;
    };

    class GetScanModeMessage : public CellularMessage
    {
      public:
        GetScanModeMessage() : CellularMessage(Type::GetScanMode)
        {}
    };

    class GetFirmwareVersionMessage : public CellularMessage
    {
      public:
        GetFirmwareVersionMessage() : CellularMessage(Type::GetFirmwareVersion)
        {}
    };

    class GetCsqMessage : public CellularMessage
    {
      public:
        GetCsqMessage() : CellularMessage(Type::GetCSQ)
        {}
    };

    class GetCregMessage : public CellularMessage
    {
      public:
        GetCregMessage() : CellularMessage(Type::GetCREG)
        {}
    };

    class GetNwinfoMessage : public CellularMessage
    {
      public:
        GetNwinfoMessage() : CellularMessage(Type::GetNWINFO)
        {}
    };

    class GetAntennaMessage : public CellularMessage
    {
      public:
        GetAntennaMessage() : CellularMessage(Type::GetAntenna)
        {}
    };

    class SetFlightModeMessage : public CellularMessage
    {
      public:
        explicit SetFlightModeMessage(bool flightModeOn)
            : CellularMessage(Type::SetFlightMode), flightModeOn(flightModeOn)
        {}
        bool flightModeOn;
    };

    class SetConnectionFrequencyMessage : public CellularMessage
    {
      public:
        explicit SetConnectionFrequencyMessage(const uint8_t &connectionFrequency)
            : CellularMessage(Type::CellularSetConnectionFrequency), connectionFrequency(connectionFrequency)
        {}
        auto getConnectionFrequency() const noexcept -> uint8_t
        {
            return connectionFrequency;
        }

      private:
        uint8_t connectionFrequency;
    };

    class PowerUpProcedureCompleteNotification : public NotificationMessage
    {
      public:
        explicit PowerUpProcedureCompleteNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::PowerUpProcedureComplete, data)
        {}
    };

    class PowerDownDeregisteringNotification : public NotificationMessage
    {
      public:
        explicit PowerDownDeregisteringNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::PowerDownDeregistering, data)
        {}
    };

    class PowerDownDeregisteredNotification : public NotificationMessage
    {
      public:
        explicit PowerDownDeregisteredNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::PowerDownDeregistered, data)
        {}
    };

    class NewIncomingSMSNotification : public NotificationMessage
    {
      public:
        explicit NewIncomingSMSNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::NewIncomingSMS, data)
        {}
    };

    class SmsDoneNotification : public NotificationMessage
    {
      public:
        explicit SmsDoneNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::SMSDone, data)
        {}
    };

    class SignalStrengthUpdateNotification : public NotificationMessage
    {
      public:
        explicit SignalStrengthUpdateNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::SignalStrengthUpdate, data)
        {}
    };

    class NetworkStatusUpdateNotification : public NotificationMessage
    {
      public:
        explicit NetworkStatusUpdateNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::NetworkStatusUpdate, data)
        {}
    };

    class UrcIncomingNotification : public NotificationMessage
    {
      public:
        explicit UrcIncomingNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::NewIncomingUrc, data)
        {}
    };

    class SetRadioOnOffMessage : public CellularMessage
    {
      public:
        explicit SetRadioOnOffMessage(bool radioOnOff) : CellularMessage(Type::RadioOnOff), radioOnOff(radioOnOff)
        {}
        auto getRadioOnOf() -> bool
        {
            return radioOnOff;
        }

      private:
        bool radioOnOff;
    };

    class SMSRejectedByOfflineNotification : public ResponseMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        SMSRejectedByOfflineNotification() : ResponseMessage(false)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::SMSRejectedByOfflineNotification,
                std::make_unique<app::manager::actions::ActionParams>());
        }
    };

    class CallRejectedByOfflineNotification : public ResponseMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        CallRejectedByOfflineNotification() : ResponseMessage(false)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender,
                app::manager::actions::CallRejectedByOfflineNotification,
                std::make_unique<app::manager::actions::ActionParams>());
        }
    };

    class RingNotification : public NotificationMessage
    {
      public:
        RingNotification() : NotificationMessage(NotificationMessage::Content::Ring)
        {}
    };

    class CallerIdNotification : public NotificationMessage
    {
      public:
        explicit CallerIdNotification(const utils::PhoneNumber::View &number)
            : NotificationMessage(NotificationMessage::Content::Ring), number(number)
        {}
        explicit CallerIdNotification(const utils::PhoneNumber &number)
            : NotificationMessage(NotificationMessage::Content::Ring), number(number.getView())
        {}
        explicit CallerIdNotification(const std::string &e164number)
            : NotificationMessage(NotificationMessage::Content::Ring), number(utils::PhoneNumber::parse(e164number))
        {}
        auto getNubmer() const -> utils::PhoneNumber::View
        {
            return number;
        }

      private:
        utils::PhoneNumber::View number;
    };

    class StateChange : public CellularMessage
    {
      public:
        const service::State::ST request;
        StateChange(const service::State::ST request = service::State::ST::Failed)
            : CellularMessage(Type::StateRequest), request(request)
        {}
    };

    class RawCommandRespAsync : public CellularMessage
    {
      public:
        RawCommandRespAsync(Type type) : CellularMessage(type){};
        virtual ~RawCommandRespAsync() = default;
        std::vector<std::string> data;
    };

    class GetSimContactsRequest : public sys::DataMessage
    {
      public:
        GetSimContactsRequest() : sys::DataMessage(MessageType::MessageTypeUninitialized){};
    };

    struct SimContact
    {
        std::string name;
        std::string number;
        SimContact(const std::string &name, const std::string &number) : name(name), number(number)
        {}
    };

    class GetSimContactsResponse : public sys::ResponseMessage
    {
      public:
        explicit GetSimContactsResponse(std::shared_ptr<std::vector<SimContact>> contacts)
            : sys::ResponseMessage(sys::ReturnCodes::Success), contacts(contacts)
        {}
        GetSimContactsResponse() : sys::ResponseMessage(sys::ReturnCodes::Failure)
        {}
        auto getContacts() -> std::shared_ptr<std::vector<SimContact>>
        {
            return contacts;
        }

      private:
        std::shared_ptr<std::vector<SimContact>> contacts;
    };

    class GetImeiRequest : public sys::DataMessage
    {
      public:
        GetImeiRequest() : sys::DataMessage(MessageType::MessageTypeUninitialized){};
    };

    class GetImeiResponse : public sys::ResponseMessage
    {
      public:
        explicit GetImeiResponse(std::shared_ptr<std::string> imei)
            : sys::ResponseMessage(sys::ReturnCodes::Success), imei(imei)
        {}
        GetImeiResponse() : sys::ResponseMessage(sys::ReturnCodes::Failure)
        {}
        auto getImei() -> std::shared_ptr<std::string>
        {
            return imei;
        }

      private:
        std::shared_ptr<std::string> imei;
    };

    class SimInsertedNotication : public sys::DataMessage
    {
      public:
        explicit SimInsertedNotication(at::SimInsertedStatus status)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), insertedStatus(status)
        {}
        auto getInsertedStatus() -> at::SimInsertedStatus
        {
            return insertedStatus;
        }

      private:
        at::SimInsertedStatus insertedStatus;
    };

    class RetryPhoneModeChangeRequest : public sys::DataMessage
    {
      public:
        RetryPhoneModeChangeRequest() : sys::DataMessage(MessageType::MessageTypeUninitialized){};
    };

    class URCCounterMessage : public sys::DataMessage
    {
      public:
        explicit URCCounterMessage(const uint32_t urcCounter)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), urcCounter(urcCounter){};
        auto getCounter()
        {
            return urcCounter;
        }

      private:
        uint32_t urcCounter;
    };

    class RetrySwitchCSQMode : public sys::DataMessage
    {
      public:
        enum class Mode
        {
            PermanentReporting,
            HybridReporting,
            HybridPolling
        };
        explicit RetrySwitchCSQMode(Mode newMode)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), mode(newMode){};
        auto getNewMode()
        {
            return mode;
        };

      private:
        Mode mode;
    };

    class RetryGetCSQ : public sys::DataMessage
    {
      public:
        RetryGetCSQ() : sys::DataMessage(MessageType::MessageTypeUninitialized){};
    };

    class CallAudioEventRequest : public sys::DataMessage
    {
      public:
        enum class EventType
        {
            Mute,
            Unmute,
            LoudspeakerOn,
            LoudspeakerOff
        };
        explicit CallAudioEventRequest(EventType event)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), eventType(event){};

        EventType eventType;
    };

    class IncomingCallMessage : public CellularMessage
    {
      public:
        IncomingCallMessage() : CellularMessage(Type::IncomingCall)
        {}
    };

    class CallStartedNotification : public sys::DataMessage
    {
      public:
        CallStartedNotification(utils::PhoneNumber phoneNumber, bool isIncoming)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), phoneNumber(phoneNumber),
              isIncoming(isIncoming){};
        utils::PhoneNumber getNumber()
        {
            return phoneNumber;
        };
        bool isCallIncoming()
        {
            return isIncoming;
        };

      private:
        utils::PhoneNumber phoneNumber;
        bool isIncoming;
    };

    class CallEndedNotification : public sys::DataMessage
    {
      public:
        explicit CallEndedNotification() : sys::DataMessage(MessageType::MessageTypeUninitialized){};
    };

    class CallAbortedNotification : public NotificationMessage
    {
      public:
        explicit CallAbortedNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::CallAborted, data)
        {}
    };

    class CallActiveNotification : public NotificationMessage, public app::manager::actions::ConvertibleToAction
    {
      public:
        explicit CallActiveNotification(const std::string &data = "")
            : NotificationMessage(NotificationMessage::Content::CallActive, data)
        {}

        [[nodiscard]] auto toAction() const -> std::unique_ptr<app::manager::ActionRequest>
        {
            return std::make_unique<app::manager::ActionRequest>(
                sender, app::manager::actions::ActivateCall, std::make_unique<app::manager::actions::CallParams>());
        }
    };

    class CallDurationNotification : public sys::DataMessage
    {
      public:
        explicit CallDurationNotification(const time_t duration)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), callDuration(duration){};
        time_t callDuration;
    };
    class CallerIdMessage : public CellularMessage
    {
      public:
        CallerIdMessage(const utils::PhoneNumber::View &number) : CellularMessage(Type::CallerId), number(number)
        {}
        CallerIdMessage(const utils::PhoneNumber &number) : CellularMessage(Type::CallerId), number(number.getView())
        {}
        CallerIdMessage(const std::string &e164number)
            : CellularMessage(Type::CallerId), number(utils::PhoneNumber::parse(e164number))
        {}

        utils::PhoneNumber::View number;
    };
    class CallOutgoingAccepted : public sys::DataMessage
    {
      public:
        explicit CallOutgoingAccepted() : sys::DataMessage(MessageType::MessageTypeUninitialized){};
    };

    class IsCallActive : public sys::DataMessage
    {};
    struct IsCallActiveResponse : public sys::ResponseMessage
    {
        explicit IsCallActiveResponse(bool active) : active{active}
        {}

        const bool active = false;
    };

    class GetVolteStateRequest : public sys::DataMessage
    {};

    struct GetVolteStateResponse : public sys::DataMessage
    {
        VolteState volteState;
        explicit GetVolteStateResponse(VolteState volteState) : volteState(volteState)
        {}
    };

    struct VolteStateNotification : public sys::DataMessage
    {
        VolteState volteState;
        explicit VolteStateNotification(VolteState volteState)
            : sys::DataMessage(MessageType::MessageTypeUninitialized), volteState(volteState)
        {}
    };

    struct SwitchVolteOnOffRequest : public sys::DataMessage
    {
        bool enable = false;
        explicit SwitchVolteOnOffRequest(bool enable)
            : DataMessage(MessageType::MessageTypeUninitialized), enable(enable)
        {}
    };
} // namespace cellular
