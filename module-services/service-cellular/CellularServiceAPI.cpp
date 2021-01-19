// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include <MessageType.hpp>
#include <Modem/TS0710/TS0710.h>
#include <PhoneNumber.hpp>
#include <Service/Bus.hpp>
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
    auto msg                          = std::make_shared<CellularCallRequestMessage>(number.getView());
    auto ret                          = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
    CellularResponseMessage *response = reinterpret_cast<CellularResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
        return true;
    }
    else {
        LOG_ERROR("Failed");
        return false;
    }
}

bool CellularServiceAPI::AnswerIncomingCall(sys::Service *serv)
{
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularAnswerIncomingCall);

    auto ret                          = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
    CellularResponseMessage *response = reinterpret_cast<CellularResponseMessage *>(ret.second.get());
    if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
        return true;
    }
    else {
        LOG_ERROR("Failed");
        return false;
    }
}

void CellularServiceAPI::HangupCall(sys::Service *serv)
{
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularHangupCall);

    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

std::string CellularServiceAPI::GetIMSI(sys::Service *serv, bool getFullIMSINumber)
{

    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularGetIMSI);

    auto ret                          = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
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

std::string CellularServiceAPI::GetOwnNumber(sys::Service *serv)
{
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularGetOwnNumber);

    auto ret                          = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
    CellularResponseMessage *response = dynamic_cast<CellularResponseMessage *>(ret.second.get());

    if (response == nullptr) {
        LOG_ERROR("CellularServiceAPI::GetOwnNumber failed");
        return std::string();
    }

    if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
        return response->data;
    }
    else {
        LOG_ERROR("CellularServiceAPI::GetOwnNumber failed");
        return std::string();
    }
}

void CellularServiceAPI::GetNetworkInfo(sys::Service *serv)
{
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularGetNetworkInfo);
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

void CellularServiceAPI::GetCurrentOperator(sys::Service *serv)
{
    std::shared_ptr<CellularGetCurrentOperatorMessage> msg = std::make_shared<CellularGetCurrentOperatorMessage>();
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

void CellularServiceAPI::StartOperatorsScan(sys::Service *serv, bool fullInfo)
{
    std::shared_ptr<CellularStartOperatorsScanMessage> msg =
        std::make_shared<CellularStartOperatorsScanMessage>(fullInfo);
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

void CellularServiceAPI::SetOperatorAutoSelect(sys::Service *serv)
{
    auto msg = std::make_shared<CellularSetOperatorAutoSelectMessage>();
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

void CellularServiceAPI::SetOperator(sys::Service *serv,
                                     at::response::cops::CopsMode mode,
                                     at::response::cops::NameFormat format,
                                     const std::string &name)
{
    auto msg = std::make_shared<CellularSetOperatorMessage>(mode, format, name);
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::SelectAntenna(sys::Service *serv, bsp::cellular::antenna antenna)
{
    auto msg     = std::make_shared<CellularAntennaRequestMessage>(MessageType::CellularSelectAntenna);
    msg->antenna = antenna;
    auto ret     = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);

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
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularSetScanMode);
    msg->data = mode;
    auto ret  = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);

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
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularGetScanMode);
    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 1000);

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
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularGetFirmwareVersion);
    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 1000);
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

bool CellularServiceAPI::GetChannel(sys::Service *serv, TS0710::Channel channel)
{
    std::shared_ptr<CellularGetChannelMessage> msg = std::make_shared<CellularGetChannelMessage>(channel);
    return sys::Bus::SendUnicast(std::move(msg), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::GetDataChannel(sys::Service *serv)
{
    return GetChannel(serv, TS0710::Channel::Data);
}

bool CellularServiceAPI::GetCSQ(sys::Service *serv, std::string &response)
{

    auto msg = std::make_shared<CellularRequestMessage>(MessageType::CellularGetCSQ);
    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
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
    auto msg = std::make_shared<CellularRequestMessage>(MessageType::CellularGetCREG);
    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
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
    auto msg = std::make_shared<CellularRequestMessage>(MessageType::CellularGetNWINFO);
    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
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
    auto msg = std::make_shared<CellularRequestMessage>(MessageType::CellularGetAntenna);
    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
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
    return sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::USSDRequest(sys::Service *serv, CellularUSSDMessage::RequestType type, std::string data)
{
    auto msg = std::make_shared<CellularUSSDMessage>(type, data);
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
    return true;
}

bool CellularServiceAPI::ChangeSimPin(sys::Service *serv,
                                      Store::GSM::SIM sim,
                                      const std::vector<unsigned int> &passcode,
                                      const std::vector<unsigned int> &pin)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularSimPukDataMessage>(sim, passcode, pin), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::SetSimCardLock(sys::Service *serv,
                                        Store::GSM::SIM sim,
                                        CellularSimCardLockDataMessage::SimCardLock lock,
                                        const std::vector<unsigned int> &pin)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularSimCardLockDataMessage>(sim, lock, pin), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::SetSimCard(sys::Service *serv, Store::GSM::SIM sim)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularChangeSimDataMessage>(sim), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::GetAPN(sys::Service *serv)
{
    return sys::Bus::SendUnicast(std::make_shared<CellularGetAPNMessage>(), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::GetAPN(sys::Service *serv, std::uint8_t contextId)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularGetAPNMessage>(contextId), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::GetAPN(sys::Service *serv, packet_data::APN::APNType type)
{
    return sys::Bus::SendUnicast(std::make_shared<CellularGetAPNMessage>(type), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::SetAPN(sys::Service *serv, packet_data::APN::Config apnConfig)
{
    auto apn = std::make_shared<packet_data::APN::Config>(std::move(apnConfig));
    return sys::Bus::SendUnicast(std::make_shared<CellularSetAPNMessage>(apn), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::SetDataTransfer(sys::Service *serv, packet_data::DataTransfer dt)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularSetDataTransferMessage>(dt), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::GetDataTransfer(sys::Service *serv)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularGetDataTransferMessage>(), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::SetVoLTE(sys::Service *serv, bool voLTE)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularChangeVoLTEDataMessage>(voLTE), ServiceCellular::serviceName, serv);
}

bool CellularServiceAPI::ChangeModulePowerState(sys::Service *serv, cellular::State::PowerState newState)
{
    return sys::Bus::SendUnicast(
        std::make_shared<CellularPowerStateChange>(newState), ServiceCellular::serviceName, serv);
}
