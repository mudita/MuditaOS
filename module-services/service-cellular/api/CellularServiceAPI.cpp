/*
 *  @file CellularServiceAPI.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "../ServiceCellular.hpp"

#include "CellularServiceAPI.hpp"

#include <PhoneNumber.hpp>
#include <Service/Bus.hpp>
#include <utf8/UTF8.hpp>

#include <memory>
#include <string>
#include <utility>

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

void CellularServiceAPI::StartOperatorsScan(sys::Service *serv)
{
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularStartOperatorsScan);
    sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 185000);
}

bool CellularServiceAPI::SelectAntenna(sys::Service *serv, bsp::cellular::antenna antenna)
{
    auto msg     = std::make_shared<CellularAntennaRequestMessage>(MessageType::CellularSelectAntenna);
    msg->antenna = antenna;
    auto ret  = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);

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
