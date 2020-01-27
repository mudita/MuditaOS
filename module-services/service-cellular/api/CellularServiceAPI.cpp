/*
 *  @file CellularServiceAPI.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "CellularServiceAPI.hpp"
#include "Service/Bus.hpp"
#include "../ServiceCellular.hpp"
#include "utf8/UTF8.hpp"

bool CellularServiceAPI::DialNumber(sys::Service* serv,const std::string& number) {
    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(MessageType::CellularDialNumber);
    msg->data = number;

    auto ret = sys::Bus::SendUnicast(msg,ServiceCellular::serviceName,serv,5000);
    CellularResponseMessage* response = reinterpret_cast<CellularResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)){
        return true;
    }
    else{
        LOG_ERROR("Failed");
        return false;
    }

}

bool CellularServiceAPI::AnswerIncomingCall(sys::Service* serv) {
    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(MessageType::CellularAnswerIncomingCall);

    auto ret = sys::Bus::SendUnicast(msg,ServiceCellular::serviceName,serv,5000);
    CellularResponseMessage* response = reinterpret_cast<CellularResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)){
        return true;
    }
    else{
        LOG_ERROR("Failed");
        return false;
    }
}

bool CellularServiceAPI::HangupCall(sys::Service* serv){
    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(MessageType::CellularHangupCall);

    auto ret = sys::Bus::SendUnicast(msg,ServiceCellular::serviceName,serv,5000);
    CellularResponseMessage* response = reinterpret_cast<CellularResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)){
        return true;
    }
    else{
        LOG_ERROR("Failed");
        return false;
    }
}

std::string CellularServiceAPI::GetIMSI(sys::Service *serv, bool getFullIMSINumber)
{

    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(MessageType::CellularGetIMSI);

    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
    CellularResponseMessage *response = dynamic_cast<CellularResponseMessage *>(ret.second.get());

    if (response == nullptr)
    {
        LOG_ERROR("Failed");
        return std::string();
    }

    if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true))
    {
        return response->data;
    }
    else
    {
        LOG_ERROR("Failed");
        return std::string();
    }
}

std::string CellularServiceAPI::GetOwnNumber(sys::Service *serv)
{
    std::shared_ptr<CellularRequestMessage> msg = std::make_shared<CellularRequestMessage>(MessageType::CellularGetOwnNumber);

    auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
    CellularResponseMessage *response = dynamic_cast<CellularResponseMessage *>(ret.second.get());

    if (response == nullptr)
    {
        LOG_ERROR("Failed");
        return std::string();
    }

    if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true))
    {
        return response->data;
    }
    else
    {
        LOG_ERROR("Failed");
        return std::string();
    }
}
