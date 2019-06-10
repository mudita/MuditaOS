
/*
 * @file DBServiceAPI.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "DBServiceAPI.hpp"

#include "../messages/DBMessage.hpp"

#include "../ServiceDB.hpp"

SettingsRecord DBServiceAPI::SettingsGet(sys::Service* serv) {

    std::shared_ptr<DBSettingsMessage> msg = std::make_shared<DBSettingsMessage>(MessageType::DBSettingsGet);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    if(ret.first == sys::ReturnCodes::Success){
        auto respMsg = ret.second;
        DBSettingsResponseMessage* settingsResponse = reinterpret_cast<DBSettingsResponseMessage*>(respMsg.get());

        if(settingsResponse->retCode == true){
            return settingsResponse->record;
        }
        else{
            return SettingsRecord{};
        }

    }
    else{
        return SettingsRecord{};
    }

}

bool DBServiceAPI::SettingsUpdate(sys::Service* serv,const SettingsRecord &rec) {
    std::shared_ptr<DBSettingsMessage> msg = std::make_shared<DBSettingsMessage>(MessageType::DBSettingsUpdate,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    if(ret.first == sys::ReturnCodes::Success){
        return true;
    }
    else{
        return false;
    }
}


bool DBServiceAPI::SMSAdd(sys::Service *serv, const SMSRecord &rec) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSAdd,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::SMSRemove(sys::Service *serv, uint32_t id) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSAdd);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::SMSUpdate(sys::Service *serv, const SMSRecord &rec) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSAdd,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

std::unique_ptr<std::vector<SMSRecord>> DBServiceAPI::SMSGetLimitOffset(sys::Service *serv,uint32_t offset, uint32_t limit) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSAdd);
    msg->offset = offset;
    msg->limit = limit;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return std::move(smsResponse->records);
    }
    else{
        return std::make_unique<std::vector<SMSRecord>>();
    }
}

ThreadRecord DBServiceAPI::ThreadGet(sys::Service *serv,uint32_t id) {
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGet);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
        return std::move((*threadResponse->records)[0]);
    }
    else{
        return ThreadRecord{};
    }
}

bool DBServiceAPI::ThreadRemove(sys::Service *serv, uint32_t id) {
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadRemove);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

std::unique_ptr<std::vector<ThreadRecord>> DBServiceAPI::ThreadGetLimitOffset(sys::Service *serv, uint32_t offset,
                                                                              uint32_t limit) {
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGetLimitOffset);
    msg->offset = offset;
    msg->limit = limit;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
        return std::move(threadResponse->records);
    }
    else{
        return std::make_unique<std::vector<ThreadRecord>>();
    }
}