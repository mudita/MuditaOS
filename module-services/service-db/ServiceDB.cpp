
/*
 * @file ServiceDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "ServiceDB.hpp"

#include "messages/DBSMSMessage.hpp"
#include "messages/DBThreadMessage.hpp"
#include "messages/DBNotificationMessage.hpp"
#include "messages/DBSettingsMessage.hpp"
#include "messages/DBSMSTemplateMessage.hpp"
#include "messages/DBContactMessage.hpp"
#include "messages/DBAlarmMessage.hpp"
#include "messages/DBNotesMessage.hpp"
#include "messages/DBCalllogMessage.hpp"
#include "messages/DBCountryCodeMessage.hpp"
#include <messages/DBServiceMessage.hpp>

#include "Database/Database.hpp"

#include "log/log.hpp"

#include <cassert>
#include <time/ScopedTime.hpp>
#include "includes/DBServiceName.hpp"
#include "messages/QueryMessage.hpp"

static const auto service_db_stack = 1024 * 24;

ServiceDB::ServiceDB() : sys::Service(service::name::db, "", service_db_stack, sys::ServicePriority::Idle)
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
    countryCodesDB.reset();
    notificationsDB.reset();
    eventsDB.reset();

    Database::deinitialize();
    LOG_INFO("[ServiceDB] Cleaning resources");
}

db::Interface *ServiceDB::getInterface(db::Interface::Name interface)
{
    switch (interface) {
    case db::Interface::Name::Settings:
        return settingsRecordInterface.get();
    case db::Interface::Name::SMS:
        return smsRecordInterface.get();
    case db::Interface::Name::SMSThread:
        return threadRecordInterface.get();
    case db::Interface::Name::SMSTemplate:
        return smsTemplateRecordInterface.get();
    case db::Interface::Name::Contact:
        return contactRecordInterface.get();
    case db::Interface::Name::Alarms:
        return alarmsRecordInterface.get();
    case db::Interface::Name::Notes:
        return notesRecordInterface.get();
    case db::Interface::Name::Calllog:
        return calllogRecordInterface.get();
    case db::Interface::Name::CountryCodes:
        return countryCodeRecordInterface.get();
    case db::Interface::Name::Notifications:
        return notificationsRecordInterface.get();
    case db::Interface::Name::Events:
        return eventsRecordInterface.get();
    case db::Interface::Name::Settings_v2:
        return settingsRecordInterface_v2.get();
    }
    return nullptr;
}

// Invoked upon receiving data message
sys::Message_t ServiceDB::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{

    std::shared_ptr<sys::ResponseMessage> responseMsg;
    auto type = static_cast<MessageType>(msgl->messageType);
    switch (type) {

    /*
     * Settings record
     */
    case MessageType::DBSettingsGet: {
        auto time        = utils::time::Scoped("DBSettingsGet");
        auto settingsRec = settingsRecordInterface->GetByID(1);
        responseMsg = std::make_shared<DBSettingsResponseMessage>(settingsRec, settingsRec.dbID == 0 ? false : true);
    } break;
    case MessageType::DBSettingsUpdate: {
        auto time              = utils::time::Scoped("DBSettingsUpdate");
        DBSettingsMessage *msg = reinterpret_cast<DBSettingsMessage *>(msgl);
        auto ret               = settingsRecordInterface->Update(msg->record);
        responseMsg            = std::make_shared<DBSettingsResponseMessage>(SettingsRecord{}, ret);
    } break;
        /*
         * SMS records
         */

    case MessageType::DBSMSAdd: {
        auto time         = utils::time::Scoped("DBSMSAdd");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret          = smsRecordInterface->Add(msg->record);
        if (ret == true) {
            // update db ID in response message
            auto record    = std::make_unique<std::vector<SMSRecord>>();
            msg->record.ID = smsRecordInterface->GetLastID();
            record->push_back(msg->record);
            LOG_INFO("SMS added, record ID: %" PRIu32, msg->record.ID);
            responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(record), ret);
            sendUpdateNotification(db::Interface::Name::SMS, db::Query::Type::Create);
        }
    } break;

    case MessageType::DBSMSRemove: {
        auto time         = utils::time::Scoped("DBSMSRemove");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret          = smsRecordInterface->RemoveByID(msg->record.ID);
        responseMsg       = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::SMS, db::Query::Type::Delete);
        auto thread = threadRecordInterface->GetByID((msg->record.threadID));
        if (!thread.isValid()) {
            LOG_DEBUG("also thread has been deleted");
            sendUpdateNotification(db::Interface::Name::SMSThread, db::Query::Type::Delete);
        }
    } break;

    case MessageType::DBSMSUpdate: {
        auto time         = utils::time::Scoped("DBSMSUpdate");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret          = smsRecordInterface->Update(msg->record);
        responseMsg       = std::make_shared<DBSMSResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::SMS, db::Query::Type::Update);
    } break;

    case MessageType::DBSMSGetSMSLimitOffset: {
        auto time         = utils::time::Scoped("DBSMSGetSMSLimitOffset");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret          = smsRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg       = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
    } break;

    case MessageType::DBSMSGetSMSLimitOffsetByThreadID: {
        auto time         = utils::time::Scoped("DBSMSGetSMSLimitOffsetByThreadID");
        DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
        auto ret          = smsRecordInterface->GetLimitOffsetByField(
            msg->offset, msg->limit, SMSRecordField::ThreadID, std::to_string(msg->id).c_str());
        responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
    } break;

    case MessageType::DBSMSGetLastRecord: {
        auto time    = utils::time::Scoped("DBSMSGetLastRecord");
        uint32_t id  = smsRecordInterface->GetLastID();
        auto rec     = smsRecordInterface->GetByID(id);
        auto records = std::make_unique<std::vector<SMSRecord>>();
        records->push_back(rec);
        responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(records), true);
        break;
    }
    case MessageType::DBSMSGetCount: {
        auto time   = utils::time::Scoped("DBSMSGetCount");
        auto ret    = smsRecordInterface->GetCount();
        responseMsg = std::make_shared<DBSMSResponseMessage>(nullptr, true, ret);
        break;
    }
        /**
         * Thread records
         */

    case MessageType::DBThreadGet: {
        auto time            = utils::time::Scoped("DBThreadGet");
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
        auto ret             = threadRecordInterface->GetByID(msg->id);
        auto records         = std::make_unique<std::vector<ThreadRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(records), ret.isValid());
    } break;

    case MessageType::DBThreadGetForContact: {
        auto time = utils::time::Scoped("DBThreadGetForContact");
        auto msg  = dynamic_cast<DBThreadMessageGet *>(msgl);
        if (!msg) {
            LOG_ERROR("ERROR wrong message sent!");
        }
        auto ret     = threadRecordInterface->GetByContact(msg->contactID);
        auto records = std::make_unique<std::vector<ThreadRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(records), ret.isValid());
    }; break;

    case MessageType::DBThreadRemove: {
        auto time            = utils::time::Scoped("DBThreadRemove");
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
        auto ret             = threadRecordInterface->RemoveByID(msg->id);
        responseMsg          = std::make_shared<DBThreadResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::SMSThread, db::Query::Type::Delete);
    } break;

    case MessageType::DBThreadGetLimitOffset: {
        auto time            = utils::time::Scoped("DBThreadGetLimitOffset");
        DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
        auto ret             = threadRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        LOG_INFO("Thread get limit offset");
        responseMsg =
            std::make_shared<DBThreadResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    } break;

    case MessageType::DBThreadGetCount: {
        auto *msg   = static_cast<DBThreadGetCountMessage *>(msgl);
        auto time   = utils::time::Scoped("DBThreadGetCountMessage");
        auto ret    = threadRecordInterface->GetCount(msg->state);
        responseMsg = std::make_shared<DBThreadResponseMessage>(nullptr, true, 0, 0, ret);
    } break;

    case MessageType::DBThreadUpdate: {
        auto time   = utils::time::Scoped("DBThreadUpdate");
        auto msg    = static_cast<DBThreadMessage *>(msgl);
        auto ret    = threadRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBThreadResponseMessage>(nullptr, true, 0, 0, ret);
        sendUpdateNotification(db::Interface::Name::SMSThread, db::Query::Type::Update);
    } break;

        /**
         * SMS templates records
         */

    case MessageType::DBSMSTemplateAdd: {
        auto time   = utils::time::Scoped("DBSMSTemplateAdd");
        auto msg    = static_cast<DBSMSTemplateMessage *>(msgl);
        auto ret    = smsTemplateRecordInterface->Add(msg->record);
        responseMsg = std::make_shared<DBSMSTemplateResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBSMSTemplateUpdate: {
        auto time   = utils::time::Scoped("DBSMSTemplateUpdate");
        auto msg    = static_cast<DBSMSTemplateMessage *>(msgl);
        auto ret    = smsTemplateRecordInterface->Update(msg->record);
        responseMsg = std::make_shared<DBSMSTemplateResponseMessage>(nullptr, ret);
    }; break;

    case MessageType::DBSMSTemplateRemove: {
        auto time   = utils::time::Scoped("DBSMSTemplateRemove");
        auto msg    = static_cast<DBSMSTemplateMessage *>(msgl);
        auto ret    = smsTemplateRecordInterface->RemoveByID(msg->id);
        responseMsg = std::make_shared<DBSMSTemplateResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBSMSTemplateGetLimitOffset: {
        auto time   = utils::time::Scoped("DBSMSTemplateGetLimitOffset");
        auto msg    = static_cast<DBSMSTemplateMessage *>(msgl);
        auto ret    = smsTemplateRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg = std::make_shared<DBSMSTemplateResponseMessage>(
            std::move(ret), true, msg->limit, msg->offset, ret->size(), type);
    } break;

    case MessageType::DBSMSTemplateGetCount: {
        auto time   = utils::time::Scoped("DBSMSTemplateGetCount");
        auto ret    = smsTemplateRecordInterface->GetCount();
        responseMsg = std::make_shared<DBSMSTemplateResponseMessage>(nullptr, true, 0, 0, ret);
    } break;

        /**
         * Contact records
         */

    case MessageType::DBContactAdd: {
        auto time             = utils::time::Scoped("DBContactAdd");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->Add(msg->record);
        responseMsg           = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Create);
    } break;

    case MessageType::DBContactGetByName: {
        auto time             = utils::time::Scoped("DBContactGetByName");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->GetByName(msg->record.primaryName, msg->record.alternativeName);
        responseMsg           = std::make_shared<DBContactResponseMessage>(
            std::move(ret), true, msg->limit, msg->offset, msg->favourite, ret->size(), type);
    } break;

    case MessageType::DBContactSearch: {
        auto time                   = utils::time::Scoped("DBContactSearch");
        DBContactSearchMessage *msg = reinterpret_cast<DBContactSearchMessage *>(msgl);
        auto ret    = contactRecordInterface->Search(msg->primaryName, msg->alternativeName, msg->number);
        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true);
    } break;

    case MessageType::DBContactGetByID: {
        auto time             = utils::time::Scoped("DBContactGetByID");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->GetByID(msg->record.ID);
        auto records          = std::make_unique<std::vector<ContactRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBContactResponseMessage>(
            std::move(records), true, msg->limit, msg->offset, msg->favourite, 1, MessageType::DBContactGetByID);
    } break;

    case MessageType::DBContactGetBySpeedDial: {
        auto time             = utils::time::Scoped("DBContactGetBySpeedDial");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->GetBySpeedDial(msg->record.speeddial);
        responseMsg           = std::make_shared<DBContactResponseMessage>(std::move(ret),
                                                                 true,
                                                                 msg->limit,
                                                                 msg->offset,
                                                                 msg->favourite,
                                                                 ret->size(),
                                                                 MessageType::DBContactGetBySpeedDial);
    } break;

    case MessageType::DBContactGetByNumber: {
        auto time             = utils::time::Scoped("DBContactGetByNumber");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->GetByNumber(msg->record.numbers[0].number);
        responseMsg           = std::make_shared<DBContactResponseMessage>(std::move(ret),
                                                                 true,
                                                                 msg->limit,
                                                                 msg->offset,
                                                                 msg->favourite,
                                                                 ret->size(),
                                                                 MessageType::DBContactGetByNumber);
    } break;

    case MessageType::DBContactMatchByNumber: {
        auto time = utils::time::Scoped("DBContactMatchByNumber");
        auto *msg = dynamic_cast<DBContactNumberMessage *>(msgl);
        auto ret  = contactRecordInterface->MatchByNumber(msg->numberView);

        if (ret.has_value()) {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(sys::ReturnCodes::Success,
                                                                           std::make_unique<ContactRecord>(*ret));
        }
        else {
            responseMsg = std::make_shared<DBContactNumberResponseMessage>(sys::ReturnCodes::Failure,
                                                                           std::unique_ptr<ContactRecord>());
        }
    } break;

    case MessageType::DBContactRemove: {
        auto time             = utils::time::Scoped("DBContactRemove");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->RemoveByID(msg->id);
        responseMsg           = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Delete);
    } break;

    case MessageType::DBContactBlock: {
        auto time           = utils::time::Scoped("DBContactBlock");
        DBContactBlock *msg = reinterpret_cast<DBContactBlock *>(msgl);
        auto ret            = contactRecordInterface->BlockByID(msg->id, msg->shouldBeBlocked);
        responseMsg         = std::make_shared<DBContactResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBContactUpdate: {
        auto time             = utils::time::Scoped("DBContactUpdate");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        auto ret              = contactRecordInterface->Update(msg->record);
        responseMsg           = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Contact, db::Query::Type::Update);
    } break;

    case MessageType::DBContactGetCount: {
        auto time             = utils::time::Scoped("DBContactGetCount");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        uint32_t ret          = 0;
        if (msg->favourite)
            ret = contactRecordInterface->GetCountFavourites();
        else
            ret = contactRecordInterface->GetCount();
        responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, true, 0, 0, msg->favourite, ret);
    } break;

    case MessageType::DBContactGetLimitOffset: {
        auto time             = utils::time::Scoped("DBContactGetLimitOffset");
        DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
        std::unique_ptr<std::vector<ContactRecord>> ret;

        ret = contactRecordInterface->GetLimitOffset(msg->offset, msg->limit);

        responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret),
                                                                 true,
                                                                 msg->limit,
                                                                 msg->offset,
                                                                 msg->favourite,
                                                                 ret->size(),
                                                                 MessageType::DBContactGetLimitOffset);

    } break;

        /**
         * Alarm records
         */

    case MessageType::DBAlarmAdd: {
        auto time           = utils::time::Scoped("DBAlarmAdd");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret            = alarmsRecordInterface->Add(msg->record);
        responseMsg         = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
        if (ret == true) {
            sendUpdateNotification(db::Interface::Name::Alarms, db::Query::Type::Create);
        }
    } break;

    case MessageType::DBAlarmRemove: {
        auto time           = utils::time::Scoped("DBAlarmRemove");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret            = alarmsRecordInterface->RemoveByID(msg->id);
        responseMsg         = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBAlarmUpdate: {
        auto time           = utils::time::Scoped("DBAlarmUpdate");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret            = alarmsRecordInterface->Update(msg->record);
        responseMsg         = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBAlarmGetCount: {
        auto time   = utils::time::Scoped("DBAlarmGetCount");
        auto ret    = alarmsRecordInterface->GetCount();
        responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, true, ret);
    } break;

    case MessageType::DBAlarmGetLimitOffset: {
        auto time           = utils::time::Scoped("DBAlarmGetLimitOffset");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret            = alarmsRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg         = std::make_shared<DBAlarmResponseMessage>(std::move(ret), true);
    } break;
    case MessageType::DBAlarmGetNext: {
        auto time           = utils::time::Scoped("DBAlarmGetNext");
        DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
        auto ret            = alarmsRecordInterface->GetNext(msg->time);
        auto records        = std::make_unique<std::vector<AlarmsRecord>>();
        records->push_back(ret);
        responseMsg = std::make_shared<DBAlarmResponseMessage>(std::move(records), ret.ID == 0 ? false : true);
    } break;

        /**
         * Notes records
         */

    case MessageType::DBNotesAdd: {
        auto time           = utils::time::Scoped("DBNotesAdd");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret            = notesRecordInterface->Add(msg->record);
        responseMsg         = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBNotesRemove: {
        auto time           = utils::time::Scoped("DBNotesRemove");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret            = notesRecordInterface->RemoveByID(msg->id);
        responseMsg         = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBNotesUpdate: {
        auto time           = utils::time::Scoped("DBNotesUpdate");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret            = notesRecordInterface->Update(msg->record);
        responseMsg         = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
    } break;

    case MessageType::DBNotesGetCount: {
        auto time   = utils::time::Scoped("DBNotesGetCount");
        auto ret    = notesRecordInterface->GetCount();
        responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, true, 0, 0, ret);
    } break;

    case MessageType::DBNotesGetLimitOffset: {
        auto time           = utils::time::Scoped("DBNotesGetLimitOffset");
        DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
        auto ret            = notesRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg =
            std::make_shared<DBNotesResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    } break;

        /**
         * Calllog records
         */

    case MessageType::DBCalllogAdd: {
        auto time             = utils::time::Scoped("DBCalllogAdd");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto record           = std::make_unique<std::vector<CalllogRecord>>();
        msg->record.ID        = DB_ID_NONE;
        auto ret              = calllogRecordInterface->Add(msg->record);
        if (ret) {
            // return the newly added record
            msg->record = calllogRecordInterface->GetByID(calllogRecordInterface->GetLastID());
        }
        record->push_back(msg->record);
        LOG_INFO("Last ID %" PRIu32, msg->record.ID);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(std::move(record), ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Create);
    } break;

    case MessageType::DBCalllogRemove: {
        auto time             = utils::time::Scoped("DBCalllogRemove");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret              = calllogRecordInterface->RemoveByID(msg->id);
        responseMsg           = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Delete);
    } break;

    case MessageType::DBCalllogUpdate: {
        auto time             = utils::time::Scoped("DBCalllogUpdate");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret              = calllogRecordInterface->Update(msg->record);
        responseMsg           = std::make_shared<DBCalllogResponseMessage>(nullptr, ret);
        sendUpdateNotification(db::Interface::Name::Calllog, db::Query::Type::Update);
    } break;

    case MessageType::DBCalllogGetCount: {
        auto *msg = dynamic_cast<DBCalllogGetCount *>(msgl);
        assert(msg);
        auto time   = utils::time::Scoped("DBCalllogGetCount");
        auto ret    = calllogRecordInterface->GetCount(msg->state);
        responseMsg = std::make_shared<DBCalllogResponseMessage>(nullptr, true, 0, 0, ret);
    } break;

    case MessageType::DBCalllogGetLimitOffset: {
        auto time             = utils::time::Scoped("DBCalllogGetLimitOffset");
        DBCalllogMessage *msg = reinterpret_cast<DBCalllogMessage *>(msgl);
        auto ret              = calllogRecordInterface->GetLimitOffset(msg->offset, msg->limit);
        responseMsg =
            std::make_shared<DBCalllogResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
    } break;

    case MessageType::DBCountryCode: {
        DBCountryCodeMessage *msg = reinterpret_cast<DBCountryCodeMessage *>(msgl);
        auto ret                  = countryCodeRecordInterface->GetByMCC(msg->mcc);
        responseMsg               = std::make_shared<DBCountryCodeResponseMessage>(ret);
    } break;

    case MessageType::DBQuery: {
        auto msg = dynamic_cast<db::QueryMessage *>(msgl);
        assert(msg);
        db::Interface *interface = getInterface(msg->getInterface());
        assert(interface != nullptr);
        auto query     = msg->getQuery();
        auto queryType = query->type;
        auto result    = interface->runQuery(std::move(query));
        responseMsg    = std::make_shared<db::QueryResponse>(std::move(result));
        sendUpdateNotification(msg->getInterface(), queryType);
    } break;

    case MessageType::DBServiceBackup: {
        auto time   = utils::time::Scoped("DBServiceBackup");
        auto msg    = static_cast<DBServiceMessageBackup *>(msgl);
        auto ret    = StoreIntoBackup({msg->backupPath});
        responseMsg = std::make_shared<DBServiceResponseMessage>(ret);
    } break;

    default:
        break;
    }

    if (responseMsg == nullptr) {
        return std::make_shared<sys::ResponseMessage>();
    }

    responseMsg->responseTo = msgl->messageType;
    return responseMsg;
}

// Invoked when timer ticked
void ServiceDB::TickHandler(uint32_t id)
{}

// Invoked during initialization
sys::ReturnCodes ServiceDB::InitHandler()
{
    Database::initialize();

    // Create databases
    settingsDB      = std::make_unique<SettingsDB>();
    contactsDB      = std::make_unique<ContactsDB>();
    smsDB           = std::make_unique<SmsDB>();
    alarmsDB        = std::make_unique<AlarmsDB>();
    notesDB         = std::make_unique<NotesDB>();
    calllogDB       = std::make_unique<CalllogDB>();
    countryCodesDB  = std::make_unique<CountryCodesDB>();
    notificationsDB = std::make_unique<NotificationsDB>();
    eventsDB        = std::make_unique<EventsDB>();

    // Create record interfaces
    settingsRecordInterface      = std::make_unique<SettingsRecordInterface>(settingsDB.get());
    contactRecordInterface       = std::make_unique<ContactRecordInterface>(contactsDB.get());
    smsRecordInterface           = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    threadRecordInterface        = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());
    smsTemplateRecordInterface   = std::make_unique<SMSTemplateRecordInterface>(smsDB.get());
    alarmsRecordInterface        = std::make_unique<AlarmsRecordInterface>(alarmsDB.get());
    notesRecordInterface         = std::make_unique<NotesRecordInterface>(notesDB.get());
    calllogRecordInterface       = std::make_unique<CalllogRecordInterface>(calllogDB.get(), contactsDB.get());
    countryCodeRecordInterface   = std::make_unique<CountryCodeRecordInterface>(countryCodesDB.get());
    notificationsRecordInterface = std::make_unique<NotificationsRecordInterface>(notificationsDB.get());
    eventsRecordInterface        = std::make_unique<EventsRecordInterface>(eventsDB.get());
    settingsRecordInterface_v2   = std::make_unique<SettingsRecordInterface_v2>(settingsDB.get());
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::DeinitHandler()
{

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDB::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[%s] PowerModeHandler: %s", this->GetName().c_str(), c_str(mode));
    return sys::ReturnCodes::Success;
}

void ServiceDB::sendUpdateNotification(db::Interface::Name interface, db::Query::Type type)
{
    auto notificationMessage = std::make_shared<db::NotificationMessage>(interface, type);
    sys::Bus::SendMulticast(notificationMessage, sys::BusChannels::ServiceDBNotifications, this);
}

bool ServiceDB::StoreIntoBackup(const std::string &backupPath)
{
    bool rc = settingsDB.get()->storeIntoFile(backupPath + "settings.db");
    rc |= contactsDB.get()->storeIntoFile(backupPath + "contacts.db");
    rc |= smsDB.get()->storeIntoFile(backupPath + "sms.db");
    rc |= alarmsDB.get()->storeIntoFile(backupPath + "alarms.db");
    rc |= notesDB.get()->storeIntoFile(backupPath + "notes.db");
    rc |= calllogDB.get()->storeIntoFile(backupPath + "calllog.db");
    rc |= eventsDB.get()->storeIntoFile(backupPath + "events.db");
    return rc;
}
