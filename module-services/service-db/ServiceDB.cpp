
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

#include "messages/DBNotificationMessage.hpp"

#include "MessageType.hpp"

#include "Database/Database.hpp"

#include "log/log.hpp"

#include "ticks.hpp"

#define SHOW_DB_ACCESS_PERF 1

const char *ServiceDB::serviceName = "ServiceDB";

class TimeStamp
{
    ssize_t timestamp;
    std::string text;

  public:
    TimeStamp(const std::string &text)
    {
#if SHOW_DB_ACCESS_PERF == 1
        this->text = text;
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
    }
    ~TimeStamp()
    {
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("%s time: %lu", text.c_str(), cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
    }
};

ServiceDB::ServiceDB() : sys::Service(serviceName, "", 1024 * 24, sys::ServicePriority::Idle)
{
    LOG_INFO("[ServiceDB] Initializing");
}

ServiceDB::~ServiceDB()
{

    settingsDB.reset();
    contactsDB.reset();
    smsDB.reset();
    alarmsDB.reset();
    notesDB.reset();

    Database::Deinitialize();
    LOG_INFO("[ServiceDB] Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceDB::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{

    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch (static_cast<MessageType>(msgl->messageType))
    {

    /*
     * Settings record
     */
    case MessageType::DBSettingsGet: {
        TimeStamp("DBSettingsGet");
        auto settingsRec = settingsRecordInterface->GetByID(1);
        responseMsg = std::make_shared<DBSettingsResponseMessage>(settingsRec, settingsRec.dbID == 0 ? false : true);
    }
    break;
    case MessageType::DBSettingsUpdate: {
        TimeStamp("DBSettingsUpdate");
        DBSettingsMessage *msg = reinterpret_cast<DBSettingsMessage *>(msgl);
        auto ret = settingsRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBSettingsResponseMessage>(SettingsRecord{}, ret);
    }
    break;

        /*
         * SMS records
         */

    case MessageType::DBSMSAdd: {
        TimeStamp("DBSMSAdd");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret = smsRecordInterface->Add(msg->record);
        if (ret == true)
        {
            // update db ID in response message
            auto record = std::make_unique<std::vector<SMSRecord>>();
            msg->record.dbID = smsRecordInterface->GetLastID();
            record->push_back(msg->record);
            LOG_INFO("SMS added, record ID: %d", msg->record.dbID);
            responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(record), ret);

            // send notification
            auto notificationMessage =
                std::make_shared<DBNotificationMessage>(MessageType::DBServiceNotification, DB::NotificatonType::Added, DB::BaseType::SmsDB);
            sys::Bus::SendMulticast(notificationMessage, sys::BusChannels::ServiceDBNotifications, this);
        }
    }
    break;

    case MessageType::DBSMSRemove: {
        TimeStamp("DBSMSRemove");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret = smsRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBSMSUpdate: {
        TimeStamp("DBSMSUpdate");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret = smsRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBSMSGetSMSLimitOffset: {
        TimeStamp("DBSMSGetSMSLimitOffset");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret = smsRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
    }
    break;

    case MessageType::DBSMSGetSMSLimitOffsetByThreadID: {
        TimeStamp("DBSMSGetSMSLimitOffsetByThreadID");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret = smsRecordInterface->GetLimitOffsetByField(msg->offset, msg->limit, SMSRecordField::ThreadID, std::to_string(msg->id).c_str());
        responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
    }
    break;

    case MessageType::DBSMSGetLastRecord:
    {
        TimeStamp("DBSMSGetLastRecord");
        uint32_t id = smsRecordInterface->GetLastID();
        auto rec = smsRecordInterface->GetByID(id);
    	auto records = std::make_unique<std::vector<SMSRecord>>();
    	records->push_back(rec);
    	responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(records), true);
    	break;
    }
    case MessageType::DBSMSGetCount: {
        TimeStamp("DBSMSGetCount");
        auto ret = smsRecordInterface->GetCount();
        responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, true, ret);
        break;
    }
        /**
         * Thread records
         */

    case MessageType::DBThreadGet: {
        TimeStamp("DBThreadGet");
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
        auto ret = threadRecordInterface->GetByID(msg->id);
        auto records = std::make_unique<std::vector<ThreadRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(records), ret.dbID == 0 ? false : true);
    }
    break;

    case MessageType::DBThreadGetForContact: {
        TimeStamp("DBThreadGetForContact");
        auto msg = dynamic_cast<DBThreadMessageGet *>(msgl);
        if (!msg)
        {
            LOG_ERROR("ERROR wrong message sent!");
        }
        auto ret = threadRecordInterface->GetByContact(msg->contactID);
        auto records = std::make_unique<std::vector<ThreadRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(records), ret.dbID == 0 ? false : true);
    };
    break;

    case MessageType::DBThreadRemove: {
        TimeStamp("DBThreadRemove");
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
        auto ret = threadRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBThreadResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBThreadGetLimitOffset: {
        TimeStamp("DBThreadGetLimitOffset");
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
        auto ret = threadRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        LOG_INFO("Thread get limit offset");
        responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    }
    break;

    case MessageType::DBThreadGetCount: {
        //  DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
        TimeStamp("DBThreadGetCount");
        auto ret = threadRecordInterface->GetCount();
        responseMsg = std::make_shared<DBThreadResponseMessage>(nullptr, true, 0, 0, ret);
    }
    break;

    case MessageType::DBContactAdd: {
        TimeStamp("DBContactAdd");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret = contactRecordInterface->Add(msg->record);
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBContactGetByName: {
        TimeStamp("DBContactGetByName");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret = contactRecordInterface->GetByName(msg->record.primaryName, msg->record.alternativeName);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetByName));
    }
    break;
    
    case MessageType::DBContactSearch: {
        TimeStamp("DBContactSearch");
        DBContactSearchMessage *msg = reinterpret_cast<DBContactSearchMessage *>(msgl);
        auto ret = contactRecordInterface->Search(msg->primaryName, msg->alternativeName, msg->number);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true);
    }
    break;
    
    case MessageType::DBContactGetByID: {
        TimeStamp("DBContactGetByID");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret = contactRecordInterface->GetByID(msg->record.dbID);
        auto records = std::make_unique<std::vector<ContactRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(records), true, msg->limit, msg->offset, msg->favourite, 1,
                                                                 static_cast<uint32_t>(MessageType::DBContactGetByID));
    }
    break;

    case MessageType::DBContactGetBySpeedDial: {
        TimeStamp("DBContactGetBySpeedDial");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret = contactRecordInterface->GetBySpeedDial(msg->record.speeddial);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetBySpeedDial));
    }
    break;

    case MessageType::DBContactGetByNumber: {
        TimeStamp("DBContactGetByNumber");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret = contactRecordInterface->GetByNumber(msg->record.number);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetByNumber));
    }
    break;

    case MessageType::DBContactRemove: {
        TimeStamp("DBContactRemove");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret = contactRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBContactBlock: {
        TimeStamp("DBContactBlock");
        DBContactBlock *msg = reinterpret_cast<DBContactBlock *>(msgl);
        auto ret = contactRecordInterface->BlockByID(msg->id, msg->shouldBeBlocked);
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;
    
    case MessageType::DBContactUpdate: {
        TimeStamp("DBContactUpdate");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret = contactRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBContactGetCount: {
        TimeStamp("DBContactGetCount");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        uint32_t ret = 0;
        if (msg->favourite)
            ret = contactRecordInterface->GetCountFavourites();
        else
            ret = contactRecordInterface->GetCount();
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, true, 0, 0, msg->favourite, ret);
    }
    break;

    case MessageType::DBContactGetLimitOffset: {
        TimeStamp("DBContactGetLimitOffset");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        std::unique_ptr<std::vector<ContactRecord>> ret;
        if (msg->favourite)
            ret = contactRecordInterface->GetLimitOffsetByField(msg->offset, msg->limit, ContactRecordField::Favourite, "1");
        else
            ret = contactRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetLimitOffset));
    }
    break;

    case MessageType::DBAlarmAdd: {
        TimeStamp("DBAlarmAdd");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret = alarmsRecordInterface->Add(msg->record);
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
        if (ret == true)
        {
            auto notificationMessage =
                std::make_shared<DBNotificationMessage>(MessageType::DBServiceNotification, DB::NotificatonType::Updated, DB::BaseType::AlarmDB);
            notificationMessage->notificationType = DB::NotificatonType::Added;
            sys::Bus::SendMulticast(notificationMessage, sys::BusChannels::ServiceDBNotifications, this);
        }
    }
    break;

    case MessageType::DBAlarmRemove: {
        TimeStamp("DBAlarmRemove");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret = alarmsRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBAlarmUpdate: {
        TimeStamp("DBAlarmUpdate");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret = alarmsRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBAlarmGetCount: {
        TimeStamp("DBAlarmGetCount");
        auto ret = alarmsRecordInterface->GetCount();
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, true, ret);
    }
    break;

    case MessageType::DBAlarmGetLimitOffset: {
        TimeStamp("DBAlarmGetLimitOffset");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret = alarmsRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg = std::make_shared<DBAlarmResponseMessage>(std::move(ret), true);
    }
    break;
    case MessageType::DBAlarmGetNext: {
        TimeStamp("DBAlarmGetNext");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret = alarmsRecordInterface->GetNext(msg->time);
        auto records = std::make_unique<std::vector<AlarmsRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBAlarmResponseMessage>(std::move(records), ret.ID == 0 ? false : true);
    }
    break;

        /****** Notes */
    case MessageType::DBNotesAdd: {
        TimeStamp("DBNotesAdd");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret = notesRecordInterface->Add(msg->record);
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBNotesRemove: {
        TimeStamp("DBNotesRemove");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret = notesRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBNotesUpdate: {
        TimeStamp("DBNotesUpdate");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret = notesRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBNotesGetCount: {
        TimeStamp("DBNotesGetCount");
        auto ret = notesRecordInterface->GetCount();
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, true, 0, 0, ret);
    }
    break;

    case MessageType::DBNotesGetLimitOffset: {
        TimeStamp("DBNotesGetLimitOffset");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret = notesRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg = std::make_shared<DBNotesResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    }
    break;

        /****** Calllog */
    case MessageType::DBCalllogAdd: {
        TimeStamp("DBCalllogAdd");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto record = std::make_unique<std::vector<CalllogRecord>>();
        msg->record.id = DB_ID_NONE;
        auto ret = calllogRecordInterface->Add(msg->record);
        if (ret)
        {
            // update db ID in response message
            msg->record.id = calllogRecordInterface->GetLastID();
        }
        record->push_back(msg->record);
        LOG_INFO("Last ID %d", msg->record.id);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(std::move(record), ret);
    }
    break;

    case MessageType::DBCalllogRemove: {
        TimeStamp("DBCalllogRemove");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret = calllogRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBCalllogUpdate: {
        TimeStamp("DBCalllogUpdate");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret = calllogRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBCalllogGetCount: {
        TimeStamp("DBCalllogGetCount");
        auto ret = calllogRecordInterface->GetCount();
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, true, 0, 0, ret);
    }
    break;

    case MessageType::DBCalllogGetLimitOffset: {
        TimeStamp("DBCalllogGetLimitOffset");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret = calllogRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    }
    break;

    default:
        // ignore this message
        return std::make_shared<sys::ResponseMessage>();
    }

    responseMsg->responseTo = static_cast<uint32_t>(msgl->messageType);
    return responseMsg;
}

// Invoked when timer ticked
void ServiceDB::TickHandler(uint32_t id)
{
}

// Invoked during initialization
sys::ReturnCodes ServiceDB::InitHandler()
{

    Database::Initialize();

    // Create databases
    settingsDB = std::make_unique<SettingsDB>();
    contactsDB = std::make_unique<ContactsDB>();
    smsDB = std::make_unique<SmsDB>();
    alarmsDB = std::make_unique<AlarmsDB>();
    notesDB = std::make_unique<NotesDB>();
    calllogDB = std::make_unique<CalllogDB>();

    // Create record interfaces
    settingsRecordInterface = std::make_unique<SettingsRecordInterface>(settingsDB.get());
    contactRecordInterface = std::make_unique<ContactRecordInterface>(contactsDB.get());
    smsRecordInterface = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    threadRecordInterface = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());
    alarmsRecordInterface = std::make_unique<AlarmsRecordInterface>(alarmsDB.get());
    notesRecordInterface = std::make_unique<NotesRecordInterface>(notesDB.get());
    calllogRecordInterface = std::make_unique<CalllogRecordInterface>(calllogDB.get());

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::DeinitHandler()
{

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[%s] PowerModeHandler: %d", this->GetName().c_str(), static_cast<uint32_t>(mode));
    return sys::ReturnCodes::Success;
}
