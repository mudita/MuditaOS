
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


ServiceDB::ServiceDB()
        : sys::Service(serviceName, "", 1024 * 24, sys::ServicePriority::Idle) {
    LOG_INFO("[ServiceDB] Initializing");

}

ServiceDB::~ServiceDB() {

    settingsDB.reset();
    contactsDB.reset();
    smsDB.reset();
    alarmsDB.reset();
    notesDB.reset();

    Database::Deinitialize();
    LOG_INFO("[ServiceDB] Cleaning resources");
}

// Invoked upon receiving data message
sys::Message_t ServiceDB::DataReceivedHandler(sys::DataMessage *msgl,sys::ResponseMessage* resp) {

    std::shared_ptr<sys::ResponseMessage> responseMsg;

    uint32_t timestamp = 0;

    switch (static_cast<MessageType >(msgl->messageType)) {

        /*
         * Settings record
         */
        case MessageType::DBSettingsGet: {
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto settingsRec = settingsRecordInterface->GetByID(1);
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBSettingsGet time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif

            responseMsg = std::make_shared<DBSettingsResponseMessage>(settingsRec,
                                                                      settingsRec.dbID == 0 ? false : true);

        }
            break;
        case MessageType::DBSettingsUpdate: {
            DBSettingsMessage *msg = reinterpret_cast<DBSettingsMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = settingsRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBSettingsUpdate time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBSMSAdd time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
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
            LOG_ERROR("DBSMSRemove time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBSMSUpdate time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBSMSGetSMSLimitOffset time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBSMSResponseMessage>(std::move(ret), true);
        }
            break;

        case MessageType::DBSMSGetSMSLimitOffsetByThreadID: {
            DBSMSMessage *msg = reinterpret_cast<DBSMSMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = smsRecordInterface->GetLimitOffsetByField(msg->offset, msg->limit,SMSRecordField::ThreadID,std::to_string(msg->id).c_str());
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBSMSGetSMSLimitOffsetByThreadID time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBThreadGet time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            auto records = std::make_unique<std::vector<ThreadRecord>>();
            records->push_back(ret);
            responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(records), ret.dbID == 0 ? false : true);
        }
            break;

        case MessageType::DBThreadRemove: {
            DBThreadMessage *msg = reinterpret_cast<DBThreadMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = threadRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBThreadRemove time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBThreadGetLimitOffset time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBThreadResponseMessage>(std::move(ret), true);
        }
            break;

        case MessageType::DBContactAdd: {
            DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            ContactRecordInterface::VerifyResult ret = contactRecordInterface->Verify((msg->record));
            if(ret == ContactRecordInterface::VerifyResult::VerifySuccess) {
                if(contactRecordInterface->Add(msg->record)) {
                    ret = ContactRecordInterface::VerifyResult::VerifySuccess;
                }
            }
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBContactAdd time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, *reinterpret_cast<uint32_t*>(&ret));
        }
            break;

        case MessageType::DBContactRemove: {
            DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = contactRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBContactRemove time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBContactUpdate time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, ret);
        }
            break;

        case MessageType::DBContactGetCount: {
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = contactRecordInterface->GetCount();
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBContactGetCount time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBContactResponseMessage>(nullptr, true,ret);
        }
            break;

        case MessageType::DBContactGetLimitOffset: {
            DBContactMessage *msg = reinterpret_cast<DBContactMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = contactRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBContactGetLimitOffset time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBContactResponseMessage>(std::move(ret), true);
        }
            break;

        case MessageType::DBAlarmAdd: {
            DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = alarmsRecordInterface->Add(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBAlarmAdd time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, ret);

            if(ret == true)
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
            LOG_ERROR("DBAlarmRemove time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBAlarmUpdate time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBAlarmGetCount time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
            responseMsg = std::make_shared<DBAlarmResponseMessage>(nullptr, true,ret);
        }
            break;

        case MessageType::DBAlarmGetLimitOffset: {
        	DBAlarmMessage *msg = reinterpret_cast<DBAlarmMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
            timestamp = cpp_freertos::Ticks::GetTicks();
#endif
            auto ret = alarmsRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
            LOG_ERROR("DBAlarmtGetLimitOffset time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
            LOG_ERROR("DBAlarmtGetNext time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
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
			LOG_ERROR("DBNotesAdd time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
				responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
			} break;

		case MessageType::DBNotesRemove: {
			DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
			timestamp = cpp_freertos::Ticks::GetTicks();
#endif
			auto ret = notesRecordInterface->RemoveByID(msg->id);
#if SHOW_DB_ACCESS_PERF == 1
			LOG_ERROR("DBNotesRemove time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
			responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
		} break;

		case MessageType::DBNotesUpdate: {
			DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
			timestamp = cpp_freertos::Ticks::GetTicks();
#endif
			auto ret = notesRecordInterface->Update(msg->record);
#if SHOW_DB_ACCESS_PERF == 1
			LOG_ERROR("DBNotesUpdate time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
			responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, ret);
		} break;

		case MessageType::DBNotesGetCount: {
#if SHOW_DB_ACCESS_PERF == 1
			timestamp = cpp_freertos::Ticks::GetTicks();
#endif
			auto ret = notesRecordInterface->GetCount();
#if SHOW_DB_ACCESS_PERF == 1
			LOG_ERROR("DBNotesGetCount time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
			responseMsg = std::make_shared<DBNotesResponseMessage>(nullptr, true, 0, 0, ret);
		} break;

		case MessageType::DBNotesGetLimitOffset: {
			DBNotesMessage *msg = reinterpret_cast<DBNotesMessage *>(msgl);
#if SHOW_DB_ACCESS_PERF == 1
			timestamp = cpp_freertos::Ticks::GetTicks();
#endif
			auto ret = notesRecordInterface->GetLimitOffset(msg->offset, msg->limit);
#if SHOW_DB_ACCESS_PERF == 1
			LOG_ERROR("DBNotesGetLimitOffset time: %lu",cpp_freertos::Ticks::GetTicks()-timestamp);
#endif
			responseMsg = std::make_shared<DBNotesResponseMessage>(std::move(ret), true, msg->limit, msg->offset, ret->size());
		} break;


        default:
            // ignore this message
            return std::make_shared<sys::ResponseMessage>();
    }


    responseMsg->responseTo = static_cast<uint32_t >(msgl->messageType);
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
    alarmsDB = std::make_unique<AlarmsDB>();
    notesDB = std::make_unique<NotesDB>();

    // Create record interfaces
    settingsRecordInterface = std::make_unique<SettingsRecordInterface>(settingsDB.get());
    contactRecordInterface = std::make_unique<ContactRecordInterface>(contactsDB.get());
    smsRecordInterface = std::make_unique<SMSRecordInterface>(smsDB.get(), contactsDB.get());
    threadRecordInterface = std::make_unique<ThreadRecordInterface>(smsDB.get(), contactsDB.get());
    alarmsRecordInterface = std::make_unique<AlarmsRecordInterface>(alarmsDB.get());
    notesRecordInterface = std::make_unique<NotesRecordInterface>(notesDB.get());

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
