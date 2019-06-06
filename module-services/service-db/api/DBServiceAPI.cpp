
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

        if(settingsResponse->retCode == 0){
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