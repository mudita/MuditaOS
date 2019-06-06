
/*
 * @file ServiceDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ServiceDB.hpp"

#include "messages/DBMessage.hpp"

#include "MessageType.hpp"

#include "Database/Database.hpp"
#include "Interface/SMSRecord.hpp"
#include "Interface/ThreadRecord.hpp"
#include "Interface/ContactRecord.hpp"
#include "Interface/SettingsRecord.hpp"
#include "log/log.hpp"


const char* ServiceDB::serviceName  = "ServiceDB";


ServiceDB::ServiceDB()
        : sys::Service(serviceName)
{
    LOG_INFO("[ServiceDB] Initializing");
}

ServiceDB::~ServiceDB(){
    LOG_INFO("[ServiceDB] Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceDB::DataReceivedHandler(sys::DataMessage* msgl) {

    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch(static_cast<MessageType >(msgl->messageType)){
        case MessageType ::DBSettingsGet:
        {
            DBSettingsMessage* msg = reinterpret_cast<DBSettingsMessage*>(msgl);
            auto settingsRec = msg->interface.GetByID(1);
            responseMsg = std::make_shared<DBSettingsResponseMessage>(settingsRec);

        }
            break;
        case MessageType ::DBSettingsUpdate:
        {
            DBSettingsMessage* msg = reinterpret_cast<DBSettingsMessage*>(msgl);
            auto settingsRec = msg->interface.Update(msg->record);

            responseMsg = std::make_shared<DBSettingsResponseMessage>(SettingsRecord{});
        }
            break;
        default:
            // ignore this message
            return std::make_shared<sys::ResponseMessage>();
    }



    return responseMsg;
}

// Invoked when timer ticked
void ServiceDB::TickHandler(uint32_t id) {
}

// Invoked during initialization
sys::ReturnCodes ServiceDB::InitHandler() {
    Database::Initialize();

    SettingsRecordInterface settingsRecordInterface;

    auto settingsRec = settingsRecordInterface.GetByID(1);



    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::DeinitHandler() {
    Database::Deinitialize();
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::WakeUpHandler() {
    return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceDB::SleepHandler() {
    return sys::ReturnCodes::Success;
}