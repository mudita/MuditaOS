// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include <MessageType.hpp>
#include <modem/mux/CellularMux.h>
#include <PhoneNumber.hpp>
#include <Service/Common.hpp>
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
    auto msg = std::make_shared<CellularCallRequestMessage>(number.getView());
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::DialEmergencyNumber(sys::Service *serv, const utils::PhoneNumber &number)
{
    auto msg = std::make_shared<CellularCallRequestMessage>(number.getView(), cellular::api::CallMode::Emergency);
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::AnswerIncomingCall(sys::Service *serv)
{
    auto msg = std::make_shared<CellularAnswerIncomingCallMessage>();

    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::HangupCall(sys::Service *serv)
{
    auto msg = std::make_shared<CellularHangupCallMessage>();
    serv->bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceCellularNotifications);
    return true;
}

bool CellularServiceAPI::DismissCall(sys::Service *serv, bool addNotificationToDB)
{
    auto msg = std::make_shared<CellularDismissCallMessage>(addNotificationToDB);
    serv->bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceCellularNotifications);
    return true;
}

std::string CellularServiceAPI::GetIMSI(sys::Service *serv, bool getFullIMSINumber)
{

    auto msg = std::make_shared<CellularGetIMSIMessage>();

    auto ret                          = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    CellularResponseMessage *response = dynamic_cast<CellularResponseMessage *>(ret.second.get());

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
    serv->connect(typeid(CellularGetOwnNumberResponseMessage), [callback](sys::Message *msg) {
        auto response = dynamic_cast<CellularGetOwnNumberResponseMessage *>(msg);
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
    serv->bus.sendUnicast(std::make_shared<CellularGetOwnNumberMessage>(), ServiceCellular::serviceName);
}

void CellularServiceAPI::GetNetworkInfo(sys::Service *serv)
{
    auto msg = std::make_shared<CellularGetNetworkInfoMessage>();
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::RequestCurrentOperatorName(sys::Service *serv)
{
    std::shared_ptr<CellularRequestCurrentOperatorNameMessage> msg =
        std::make_shared<CellularRequestCurrentOperatorNameMessage>();
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::StartOperatorsScan(sys::Service *serv, bool fullInfo)
{
    std::shared_ptr<CellularStartOperatorsScanMessage> msg =
        std::make_shared<CellularStartOperatorsScanMessage>(fullInfo);
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::SetOperatorAutoSelect(sys::Service *serv)
{
    auto msg = std::make_shared<CellularSetOperatorAutoSelectMessage>();
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

void CellularServiceAPI::SetOperator(sys::Service *serv,
                                     at::response::cops::CopsMode mode,
                                     at::response::cops::NameFormat format,
                                     const std::string &name)
{
    auto msg = std::make_shared<CellularSetOperatorMessage>(mode, format, name);
    serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::SelectAntenna(sys::Service *serv, bsp::cellular::antenna antenna)
{
    auto msg     = std::make_shared<CellularAntennaRequestMessage>();
    msg->antenna = antenna;
    auto ret     = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);

    CellularResponseMessage *response = dynamic_cast<CellularResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
            return true;
        }
    }

    return false;
}

bool CellularServiceAPI::SetScanMode(sys::Service *serv, std::string mode)
{
    auto msg = std::make_shared<CellularSetScanModeMessage>(mode);
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);

    CellularResponseMessage *response = dynamic_cast<CellularResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
            return true;
        }
    }
    return false;
}
bool CellularServiceAPI::GetScanMode(sys::Service *serv)
{
    auto msg = std::make_shared<CellularGetScanModeMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 1000);

    CellularResponseMessage *response = dynamic_cast<CellularResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::GetFirmwareVersion(sys::Service *serv, std::string &response)
{
    auto msg = std::make_shared<CellularGetFirmwareVersionMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 1000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto celResponse = std::dynamic_pointer_cast<CellularResponseMessage>(ret.second);
        if ((celResponse != nullptr) && (celResponse->retCode == true)) {
            LOG_DEBUG("Modem Firmware: %s", celResponse->data.c_str());
            response = celResponse->data;
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::GetChannel(sys::Service *serv, CellularMux::Channel channel)
{
    std::shared_ptr<CellularGetChannelMessage> msg = std::make_shared<CellularGetChannelMessage>(channel);
    return serv->bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
}

bool CellularServiceAPI::GetDataChannel(sys::Service *serv)
{
    return GetChannel(serv, CellularMux::Channel::Data);
}

bool CellularServiceAPI::GetCSQ(sys::Service *serv, std::string &response)
{

    auto msg = std::make_shared<CellularGetCsqMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<CellularResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->data;
            return true;
        }
    }
    return false;
}
bool CellularServiceAPI::GetCREG(sys::Service *serv, std::string &response)
{
    auto msg = std::make_shared<CellularGetCregMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<CellularResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->data;
            return true;
        }
    }
    return false;
}
bool CellularServiceAPI::GetQNWINFO(sys::Service *serv, std::string &response)
{
    auto msg = std::make_shared<CellularGetNwinfoMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<CellularResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->data;
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::GetAntenna(sys::Service *serv, bsp::cellular::antenna &response)
{
    auto msg = std::make_shared<CellularGetAntennaMessage>();
    auto ret = serv->bus.sendUnicastSync(msg, ServiceCellular::serviceName, 5000);
    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<CellularAntennaResponseMessage>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->antenna;
            return true;
        }
    }
    return false;
}

bool CellularServiceAPI::TransmitDtmfTones(sys::Service *serv, uint32_t digit)
{
    auto msg = std::make_shared<CellularDtmfRequestMessage>(digit);
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
}

bool CellularServiceAPI::USSDRequest(sys::Service *serv, CellularUSSDMessage::RequestType type, std::string data)
{
    auto msg = std::make_shared<CellularUSSDMessage>(type, data);
    return serv->bus.sendUnicast(msg, ServiceCellular::serviceName);
    ;
}

bool CellularServiceAPI::GetAPN(sys::Service *serv)
{
    return serv->bus.sendUnicast(std::make_shared<CellularGetAPNMessage>(), ServiceCellular::serviceName);
}

bool CellularServiceAPI::GetAPN(sys::Service *serv, std::uint8_t contextId)
{
    return serv->bus.sendUnicast(std::make_shared<CellularGetAPNMessage>(contextId), ServiceCellular::serviceName);
}

bool CellularServiceAPI::GetAPN(sys::Service *serv, packet_data::APN::APNType type)
{
    return serv->bus.sendUnicast(std::make_shared<CellularGetAPNMessage>(type), ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetAPN(sys::Service *serv, packet_data::APN::Config apnConfig)
{
    auto apn = std::make_shared<packet_data::APN::Config>(std::move(apnConfig));
    return serv->bus.sendUnicast(std::make_shared<CellularSetAPNMessage>(apn), ServiceCellular::serviceName);
}

bool CellularServiceAPI::NewAPN(sys::Service *serv, packet_data::APN::Config apnConfig)
{
    auto apn = std::make_shared<packet_data::APN::Config>(std::move(apnConfig));
    return serv->bus.sendUnicast(std::make_shared<CellularNewAPNMessage>(apn), ServiceCellular::serviceName);
}

bool CellularServiceAPI::DeleteAPN(sys::Service *serv, std::uint8_t contextId)
{
    auto emptyApn       = std::make_shared<packet_data::APN::Config>();
    emptyApn->contextId = contextId;
    return serv->bus.sendUnicast(std::make_shared<CellularSetAPNMessage>(emptyApn), ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetDataTransfer(sys::Service *serv, packet_data::DataTransfer dt)
{
    return serv->bus.sendUnicast(std::make_shared<CellularSetDataTransferMessage>(dt), ServiceCellular::serviceName);
}

bool CellularServiceAPI::GetDataTransfer(sys::Service *serv)
{
    return serv->bus.sendUnicast(std::make_shared<CellularGetDataTransferMessage>(), ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetVoLTE(sys::Service *serv, bool voLTE)
{
    return serv->bus.sendUnicast(std::make_shared<CellularChangeVoLTEDataMessage>(voLTE), ServiceCellular::serviceName);
}

bool CellularServiceAPI::ChangeModulePowerState(sys::Service *serv, cellular::service::State::PowerState newState)
{
    return serv->bus.sendUnicast(std::make_shared<CellularPowerStateChange>(newState), ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetFlightMode(sys::Service *serv, bool flightModeOn)
{
    return serv->bus.sendUnicast(std::make_shared<CellularSetFlightModeMessage>(flightModeOn),
                                 ServiceCellular::serviceName);
}

bool CellularServiceAPI::SetConnectionFrequency(sys::Service *serv, uint8_t connectionFrequency)
{
    return serv->bus.sendUnicast(std::make_shared<CellularSetConnectionFrequencyMessage>(connectionFrequency),
                                 ServiceCellular::serviceName);
}
