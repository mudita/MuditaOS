// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include <MessageType.hpp>
#include <modem/mux/CellularMux.h>
#include <PhoneNumber.hpp>
#include <system/Common.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <log/log.hpp>

#include <memory>
#include <string>
#include <utility>

namespace sys
{
    class Service;
} // namespace sys

bool CellularServiceAPI::DialNumber(sys::Service *serv, const utils::PhoneNumber &number)
{
    auto msg = std::make_shared<cellular::CallRequestMessage>(number.getView());
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::DialEmergencyNumber(sys::Service *serv, const utils::PhoneNumber &number)
{
    auto msg = std::make_shared<cellular::CallRequestMessage>(number.getView(), cellular::api::CallMode::Emergency);
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::AnswerIncomingCall(sys::Service *serv)
{
    auto msg = std::make_shared<cellular::AnswerIncomingCallMessage>();

    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::HangupCall(sys::Service *serv)
{
    auto msg = std::make_shared<cellular::HangupCallMessage>();
    serv->bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceCellularNotifications);
    return true;
}

std::string CellularServiceAPI::GetIMSI(sys::Service *serv, bool getFullIMSINumber)
{

    auto msg = std::make_shared<cellular::GetIMSIMessage>();

    auto ret                            = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    cellular::ResponseMessage *response = dynamic_cast<cellular::ResponseMessage *>(ret.second.get());

    if (response == nullptr) {
        LOG_ERROR("CellularServiceAPI::GetIMSI failed");
        return std::string();
    }

    if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
        return response->data;
    }
    else {
        LOG_ERROR("CellularServiceAPI::GetIMSI failed");
        return std::string();
    }
}

void CellularServiceAPI::SubscribeForOwnNumber(sys::Service *serv, std::function<void(const std::string &)> callback)
{
    serv->connect(typeid(cellular::GetOwnNumberResponseMessage), [callback](sys::Message *msg) {
        auto response = dynamic_cast<cellular::GetOwnNumberResponseMessage *>(msg);
        if (response != nullptr && response->retCode) {
            callback(response->data);
        }
        else {
            LOG_ERROR("Getting own number failed");
            callback(std::string());
        }
        return sys::MessageNone{};
    });
}

void CellularServiceAPI::RequestForOwnNumber(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<cellular::GetOwnNumberMessage>(), ServiceCellular::serviceName);
}

void CellularServiceAPI::GetNetworkInfo(sys::Service *serv)
{
    auto msg = std::make_shared<cellular::GetNetworkInfoMessage>();
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::RequestCurrentOperatorName(sys::Service *serv)
{
    auto msg = std::make_shared<cellular::RequestCurrentOperatorNameMessage>();
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::StartOperatorsScan(sys::Service *serv, bool fullInfo)
{
    auto msg = std::make_shared<cellular::StartOperatorsScanMessage>(fullInfo);
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::SetOperatorAutoSelect(sys::Service *serv)
{
    auto msg = std::make_shared<cellular::SetOperatorAutoSelectMessage>();
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::SetOperator(sys::Service *serv,
                                     at::response::cops::CopsMode mode,
                                     at::response::cops::NameFormat format,
                                     const std::string &name)
{
    auto msg = std::make_shared<cellular::SetOperatorMessage>(mode, format, name);
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::SelectAntenna(sys::Service *serv, bsp::cellular::antenna antenna)
{
    auto msg     = std::make_shared<cellular::AntennaRequestMessage>();
    msg->antenna = antenna;
    auto ret     = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);

    auto *response = dynamic_cast<cellular::ResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
            return true;
        }
    }

    return false;
}

bool CellularServiceAPI::SetScanMode(sys::Service *serv, std::string mode)
{
    auto msg = std::make_shared<cellular::SetScanModeMessage>(mode);
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);

    auto *response = dynamic_cast<cellular::ResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
            return true;
        }
    }
    return false;
}
bool CellularServiceAPI::GetScanMode(sys::Service *serv)
{
    auto msg = std::make_shared<cellular::GetScanModeMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 1000);

    auto *response = dynamic_cast<cellular::ResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::GetFirmwareVersion(sys::Service *serv, std::string &response)
{
    auto msg = std::make_shared<cellular::GetFirmwareVersionMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 1000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto celResponse = std::dynamic_pointer_cast<cellular::ResponseMessage>(ret.second);
        if ((celResponse != nullptr) && (celResponse->retCode == true)) {
            LOG_DEBUG("Modem Firmware: %s", celResponse->data.c_str());
            response = celResponse->data;
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::GetCSQ(sys::Service *serv, std::string &response)
{

    auto msg = std::make_shared<cellular::GetCsqMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<cellular::ResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->data;
            return true;
        }
    }
    return false;
}
bool CellularServiceAPI::GetCREG(sys::Service *serv, std::string &response)
{
    auto msg = std::make_shared<cellular::GetCregMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<cellular::ResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->data;
            return true;
        }
    }
    return false;
}
bool CellularServiceAPI::GetQNWINFO(sys::Service *serv, std::string &response)
{
    auto msg = std::make_shared<cellular::GetNwinfoMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<cellular::ResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->data;
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::GetAntenna(sys::Service *serv, bsp::cellular::antenna &response)
{
    auto msg = std::make_shared<cellular::GetAntennaMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<cellular::AntennaResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->antenna;
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::TransmitDtmfTones(sys::Service *serv, DTMFCode code)
{
    auto msg = std::make_shared<cellular::DtmfRequestMessage>(code);
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::USSDRequest(sys::Service *serv, cellular::USSDMessage::RequestType type, std::string data)
{
    auto msg = std::make_shared<cellular::USSDMessage>(type, data);
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
    ;
}

bool CellularServiceAPI::GetAPN(sys::Service *serv)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::GetAPNMessage>(), ServiceCellular::serviceName);
}

bool CellularServiceAPI::GetAPN(sys::Service *serv, std::uint8_t contextId)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::GetAPNMessage>(contextId), ServiceCellular::serviceName);
}

bool CellularServiceAPI::GetAPN(sys::Service *serv, packet_data::APN::APNType type)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::GetAPNMessage>(type), ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetAPN(sys::Service *serv, packet_data::APN::Config apnConfig)
{
    auto apn = std::make_shared<packet_data::APN::Config>(std::move(apnConfig));
    return serv->bus.sendUnicast(std::make_shared<cellular::SetAPNMessage>(apn), ServiceCellular::serviceName);
}

bool CellularServiceAPI::NewAPN(sys::Service *serv, packet_data::APN::Config apnConfig)
{
    auto apn = std::make_shared<packet_data::APN::Config>(std::move(apnConfig));
    return serv->bus.sendUnicast(std::make_shared<cellular::NewAPNMessage>(apn), ServiceCellular::serviceName);
}

bool CellularServiceAPI::DeleteAPN(sys::Service *serv, std::uint8_t contextId)
{
    auto emptyApn       = std::make_shared<packet_data::APN::Config>();
    emptyApn->contextId = contextId;
    return serv->bus.sendUnicast(std::make_shared<cellular::SetAPNMessage>(emptyApn), ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetDataTransfer(sys::Service *serv, packet_data::DataTransfer dt)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::SetDataTransferMessage>(dt), ServiceCellular::serviceName);
}

bool CellularServiceAPI::GetDataTransfer(sys::Service *serv)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::GetDataTransferMessage>(), ServiceCellular::serviceName);
}

bool CellularServiceAPI::ChangeModulePowerState(sys::Service *serv, cellular::service::State::PowerState newState)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::PowerStateChange>(newState), ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetFlightMode(sys::Service *serv, bool flightModeOn)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::SetFlightModeMessage>(flightModeOn),
                                 ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetConnectionFrequency(sys::Service *serv, uint8_t connectionFrequency)
{
    return serv->bus.sendUnicast(std::make_shared<cellular::SetConnectionFrequencyMessage>(connectionFrequency),
                                 ServiceCellular::serviceName);
}

bool CellularServiceAPI::CallAudioMuteEvent(sys::Service *serv)
{
    return serv->bus.sendUnicast(
        std::make_shared<cellular::CallAudioEventRequest>(cellular::CallAudioEventRequest::EventType::Mute),
        ServiceCellular::serviceName);
}

bool CellularServiceAPI::CallAudioUnmuteEvent(sys::Service *serv)
{
    return serv->bus.sendUnicast(
        std::make_shared<cellular::CallAudioEventRequest>(cellular::CallAudioEventRequest::EventType::Unmute),
        ServiceCellular::serviceName);
}

bool CellularServiceAPI::CallAudioLoudspeakerOnEvent(sys::Service *serv)
{
    return serv->bus.sendUnicast(
        std::make_shared<cellular::CallAudioEventRequest>(cellular::CallAudioEventRequest::EventType::LoudspeakerOn),
        ServiceCellular::serviceName);
}

bool CellularServiceAPI::CallAudioLoudspeakerOffEvent(sys::Service *serv)
{
    return serv->bus.sendUnicast(
        std::make_shared<cellular::CallAudioEventRequest>(cellular::CallAudioEventRequest::EventType::LoudspeakerOff),
        ServiceCellular::serviceName);
}
