
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

    uint32_t timestamp = 0;

    switch (static_cast<MessageType>(msgl->messageType))
    {

    /*
     * Settings record
     */
    case MessageType::DBSettingsGet: {
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto settingsRec = settingsRecordInterface->GetByID(1);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBSettingsGet time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif

        responseMsg = std::make_shared<DBSettingsResponseMessage>(settingsRec, settingsRec.dbID == 0 ? false : true);
    }
    break;
    case MessageType::DBSettingsUpdate: {
        DBSettingsMessage *msg = reinterpret_cast<DBSettingsMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = settingsRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBSettingsUpdate time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif

        responseMsg = std::make_shared<DBSettingsResponseMessage>(SettingsRecord{}, ret);
    }
    break;

        /*
         * SMS records
         */

    case MessageType::DBSMSAdd: {
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = smsRecordInterface->Add(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBSMSAdd time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        LOG_INFO("SMS ID %d", smsRecordInterface->GetLastID());
        responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBSMSRemove: {
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = smsRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBSMSRemove time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBSMSUpdate: {
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = smsRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBSMSUpdate time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBSMSGetSMSLimitOffset: {
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = smsRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBSMSGetSMSLimitOffset time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
    }
    break;

    case MessageType::DBSMSGetSMSLimitOffsetByThreadID: {
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = smsRecordInterface->GetLimitOffsetByField(msg->offset, msg->limit, SMSRecordField::ThreadID, std::to_string(msg->id).c_str());
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBSMSGetSMSLimitOffsetByThreadID time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
    }
    break;

        /**
         * Thread records
         */

    case MessageType::DBThreadGet: {
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = threadRecordInterface->GetByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBThreadGet time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        auto records = std::make_unique<std::vector<ThreadRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(records), ret.dbID == 0 ? false : true);
    }
    break;

    case MessageType::DBThreadGetForContact: {
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
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = threadRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBThreadRemove time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBThreadResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBThreadGetLimitOffset: {
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = threadRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBThreadGetLimitOffset time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        LOG_INFO("Thread get limit offset");
        responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    }
    break;

    case MessageType::DBThreadGetCount: {
        //  DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = threadRecordInterface->GetCount();
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBThreadGetCount time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        // DBThreadResponseMessage(std::unique_ptr<std::vector<ThreadRecord>> rec,uint32_t retCode=0,uint32_t count=0,uint32_t respTo=0)
        responseMsg = std::make_shared<DBThreadResponseMessage>(nullptr, true, 0, 0, ret);
    }
    break;

    case MessageType::DBContactAdd: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->Add(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactAdd time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBContactGetByName: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->GetByName(msg->record.primaryName, msg->record.alternativeName);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactGetByName time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetByName));
    }
    break;
    
    case MessageType::DBContactSearch: {
        DBContactSearchMessage *msg = reinterpret_cast<DBContactSearchMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->Search(msg->primaryName, msg->alternativeName, msg->number);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactSearch time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true);
    }
    break;
    
    case MessageType::DBContactGetByID: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->GetByID(msg->record.dbID);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactGetByName time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        auto records = std::make_unique<std::vector<ContactRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(records), true, msg->limit, msg->offset, msg->favourite, 1,
                                                                 static_cast<uint32_t>(MessageType::DBContactGetByID));
    }
    break;

    case MessageType::DBContactGetBySpeedDial: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->GetBySpeedDial(msg->record.speeddial);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactGetBySpeedDial time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetBySpeedDial));
    }
    break;

    case MessageType::DBContactGetByNumber: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->GetByNumber(msg->record.number);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactGetByNumber time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetByNumber));
    }
    break;

    case MessageType::DBContactRemove: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactRemove time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBContactBlock: {
        DBContactBlock *msg = reinterpret_cast<DBContactBlock *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->BlockByID(msg->id, msg->shouldBeBlocked);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactBlock time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;
    
    case MessageType::DBContactUpdate: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = contactRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactUpdate time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBContactGetCount: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif

        uint32_t ret = 0;
        if (msg->favourite)
            ret = contactRecordInterface->GetCountFavourites();
        else
            ret = contactRecordInterface->GetCount();
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactGetCount time: %lu favourites: %s", (cpp_freertos::Ticks::GetTicks() - timestamp), (msg->favourite == true ? "TRUE" : "FALSE"));
#endif
        // std::unique_ptr<std::vector<ContactRecord>> rec,uint32_t retCode=0,uint32_t  count=0,uint32_t respTo=0, bool favourite = false
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, true, 0, 0, msg->favourite, ret);
    }
    break;

    case MessageType::DBContactGetLimitOffset: {
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        std::unique_ptr<std::vector<ContactRecord>> ret;
        if (msg->favourite)
            ret = std::move(contactRecordInterface->GetLimitOffsetByField(msg->offset, msg->limit, ContactRecordField::Favourite, "1"));
        else
            ret = std::move(contactRecordInterface->GetLimitOffset(msg->offset, msg->limit));
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBContactGetLimitOffset offset: %d, limit: %d size: %d, time: %lu favourites: %s", msg->offset, msg->limit, ret->size(),
                  cpp_freertos::Ticks::GetTicks() - timestamp, (msg->favourite == true ? "TRUE" : "FALSE"));
#endif

        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(),
                                                                 static_cast<uint32_t>(MessageType::DBContactGetLimitOffset));
    }
    break;

    case MessageType::DBAlarmAdd: {
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = alarmsRecordInterface->Add(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBAlarmAdd time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);

        if (ret == true)
        {
            auto notificationMessage = std::make_shared<DBNotificationMessage>(MessageType::DBAlarmUpdateNotification);
            notificationMessage->notificationType = DBNotificatonType::Updated;
            sys::Bus::SendMulticast(notificationMessage, sys::BusChannels::ServiceDatabaseAlarmNotifications, this);
        }
    }
    break;

    case MessageType::DBAlarmRemove: {
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = alarmsRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBAlarmRemove time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBAlarmUpdate: {
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = alarmsRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBAlarmUpdate time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBAlarmGetCount: {
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = alarmsRecordInterface->GetCount();
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBAlarmGetCount time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, true, ret);
    }
    break;

    case MessageType::DBAlarmGetLimitOffset: {
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = alarmsRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBAlarmtGetLimitOffset time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBAlarmResponseMessage>(std::move(ret), true);
    }
    break;
    case MessageType::DBAlarmGetNext: {
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = alarmsRecordInterface->GetNext(msg->time);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBAlarmtGetNext time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        auto records = std::make_unique<std::vector<AlarmsRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBAlarmResponseMessage>(std::move(records), ret.ID == 0 ? false : true);
    }
    break;

        /****** Notes */
    case MessageType::DBNotesAdd: {
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = notesRecordInterface->Add(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBNotesAdd time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBNotesRemove: {
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = notesRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBNotesRemove time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBNotesUpdate: {
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = notesRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBNotesUpdate time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBNotesGetCount: {
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = notesRecordInterface->GetCount();
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBNotesGetCount time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, true, 0, 0, ret);
    }
    break;

    case MessageType::DBNotesGetLimitOffset: {
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = notesRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBNotesGetLimitOffset time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBNotesResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    }
    break;

        /****** Calllog */
    case MessageType::DBCalllogAdd: {
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = calllogRecordInterface->Add(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBCalllogAdd time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBCalllogRemove: {
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = calllogRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBCalllogRemove time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBCalllogUpdate: {
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = calllogRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBCalllogUpdate time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
    }
    break;

    case MessageType::DBCalllogGetCount: {
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = calllogRecordInterface->GetCount();
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBCalllogGetCount time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, true, 0, 0, ret);
    }
    break;

    case MessageType::DBCalllogGetLimitOffset: {
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
        timestamp = cpp_freertos::Ticks::GetTicks();
#endif
        auto ret = calllogRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
        LOG_DEBUG("DBCalllogGetLimitOffset time: %lu", cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
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
