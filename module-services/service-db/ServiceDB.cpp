
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

#include "log/log.hpp"


const char *ServiceDB::serviceName = "ServiceDB";


ServiceDB::ServiceDB()
        : sys::Service(serviceName, 1024 * 24, sys::ServicePriority::Idle) {
    LOG_INFO("[ServiceDB] Initializing");

}

ServiceDB::~ServiceDB() {

    Database::Deinitialize();
    LOG_INFO("[ServiceDB] Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceDB::DataReceivedHandler(sys::DataMessage *msgl) {

    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch (static_cast<MessageType >(msgl->messageType)) {

        /*
         * Settings record
         */
        case MessageType::DBSettingsGet: {
            auto settingsRec = settingsRecordInterface->GetByID(1);
            responseMsg = std::make_shared<DBSettingsResponseMessage>(settingsRec,
                                                                      settingsRec.dbID == 0 ? false : true);

        }
            break;
        case MessageType::DBSettingsUpdate: {
            DBSettingsMessage *msg = reinterpret_cast<DBSettingsMessage *>(msgl);
            auto ret = settingsRecordInterface->Update(msg->record);

            responseMsg = std::make_shared<DBSettingsResponseMessage>(SettingsRecord{}, ret);
        }
            break;


        /*
         * SMS records
         */

        case MessageType ::DBSMSAdd:
        {
            DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
            auto ret = smsRecordInterface->Add(msg->record);
            responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
        }
        break;

        case MessageType ::DBSMSRemove:
        {
            DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
            auto ret = smsRecordInterface->RemoveByID(msg->id);
            responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
        }
        break;

        case MessageType ::DBSMSUpdate:
        {
            DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
            auto ret = smsRecordInterface->Update(msg->record);
            responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
        }
            break;

        case MessageType ::DBSMSGetSMSLimitOffset:
        {
            DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);

            auto ret = smsRecordInterface->GetLimitOffset(msg->offset,msg->limit);
            responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
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

    // Create databases
    settingsDB = std::make_unique<SettingsDB>();
    contactsDB = std::make_unique<ContactsDB>();
    smsDB = std::make_unique<SmsDB>();

    // Create record interfaces
    settingsRecordInterface = std::make_unique<SettingsRecordInterface>(settingsDB.get());
    contactRecordInterface = std::make_unique<ContactRecordInterface>(contactsDB.get());
    smsRecordInterface = std::make_unique<SMSRecordInterface>(smsDB.get(),contactsDB.get());
    threadRecordInterface = std::make_unique<ThreadRecordInterface>(smsDB.get(),contactsDB.get());

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::DeinitHandler() {

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::WakeUpHandler() {
    return sys::ReturnCodes::Success;
}


sys::ReturnCodes ServiceDB::SleepHandler() {
    return sys::ReturnCodes::Success;
}