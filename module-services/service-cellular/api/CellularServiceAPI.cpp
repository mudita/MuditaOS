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
    auto msg                          = std::make_shared<CellularCallRequestMessage>(number.makeView());
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

bool CellularServiceAPI::SelectAntenna(sys::Service *serv, uint8_t antenna)
{
    std::shared_ptr<CellularRequestMessage> msg =
        std::make_shared<CellularRequestMessage>(MessageType::CellularSelectAntenna);
    msg->data = std::to_string(antenna);
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
