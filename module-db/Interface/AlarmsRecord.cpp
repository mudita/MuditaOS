/*
 * AlarmsRecord.cpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#include "AlarmsRecord.hpp"
#include "ThreadRecord.hpp"

AlarmsRecordInterface::AlarmsRecordInterface(AlarmsDB* alarmsDb): alarmsDB(alarmsDb) {

}

AlarmsRecordInterface::~AlarmsRecordInterface() {

}

bool AlarmsRecordInterface::Add(const AlarmsRecord &rec) {

    uint32_t contactID = 0;

//    //ContactRecordInterface contactInterface(contactsDB);
//    auto alarmsRec = alarmsInterface.GetLimitOffsetByField(0, 1, AlarmsRecordField::Number, rec.number.c_str());
//
//    // Contact not found, create one
//    if (AlarmstRec->size() == 0) {
//        alarmsInterface.Add(AlarmsRecord{
//                .numberUser=rec.number,
//                .numberE164=rec.number,
//                .contactType=ContactType::MESSAGE,
//        });
//
//        contactRec = contactInterface.GetLimitOffsetByField(0, 1, ContactRecordField::NumberE164,
//                                                                 rec.number.c_str());
//    }
//    contactID = (*contactRec)[0].dbID;
//
//
//
//    // Search for a thread with specified contactID
//    uint32_t threadID =0;
//    ThreadRecordInterface threadInterface(smsDB,contactsDB);
//    auto threadRec = threadInterface.GetLimitOffsetByField(0, 1, ThreadRecordField::ContactID,
//                                                           std::to_string(contactID).c_str());
//
//    // Thread not found, create one
//    if (threadRec->size() == 0) {
//
//        threadInterface.Add(ThreadRecord{
//                .contactID=contactID,
//        });
//
//        threadRec = threadInterface.GetLimitOffsetByField(0,1,ThreadRecordField::ContactID,std::to_string(contactID).c_str());
//
//    }
//    threadID = (*threadRec)[0].dbID;

    // Create SMS
    alarmsDB->alarms.Add(AlarmsTableRow{
    		.time = rec.time,
    		.snooze = rec.snooze,
			.status = rec.status,
			.path = rec.path
    });

    //TODO: error check

//

    return true;
}

uint32_t AlarmsRecordInterface::GetCount() {
    return alarmsDB->alarms.GetCount();
}

std::unique_ptr<std::vector<AlarmsRecord>> AlarmsRecordInterface::GetLimitOffsetByField(uint32_t offset, uint32_t limit,
                                                                                  AlarmsRecordField field,
                                                                                  const char *str) {
    auto records = std::make_unique<std::vector<AlarmsRecord>>();

    std::vector<AlarmsTableRow> alarm;

    switch(field){
        case AlarmsRecordField::Time:
        	alarm = alarmsDB->alarms.GetLimitOffsetByField(offset,limit,AlarmsTableFields::Status,str);
            break;

        case AlarmsRecordField::Snooze:
        	alarm = alarmsDB->alarms.GetLimitOffsetByField(offset,limit,AlarmsTableFields::Snooze,str);
            break;
        case AlarmsRecordField::Status:
        	alarm = alarmsDB->alarms.GetLimitOffsetByField(offset,limit,AlarmsTableFields::Snooze,str);
        	break;

        default:
            return records;
    }


//    ContactRecordInterface contactInterface(contactsDB);
//    for(const auto &w : smses){
//
//        auto contactRec = contactInterface.GetByID(w.contactID);
//
//        records->push_back({
//                                   .dbID=w.ID,
//                                   .date=w.date,
//                                   .dateSent=w.dateSent,
//                                   .errorCode=w.errorCode,
//                                   .number=contactRec.numberE164,// TODO: or numberUser?
//                                   .body=w.body,
//                                   .isRead=w.isRead,
//                                   .type=w.type,
//                                   .threadID=w.threadID,
//                                   .contactID=w.contactID
//
//                           });
//    }

    return records;
}


std::unique_ptr<std::vector<AlarmsRecord>> AlarmsRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto alarm = alarmsDB->alarms.GetLimitOffset(offset,limit);

    auto records = std::make_unique<std::vector<AlarmsRecord>>();

    AlarmsRecordInterface alarmsInterface(alarmsDB);
    for(const auto &w : alarm){

        auto alarmsRec = alarmsInterface.GetByID(w.ID);

        records->push_back({
           .ID=w.ID,
           .time = w.time,
		   .snooze = w.snooze,
		   .status = w.status,
		   .path = w.path,
        });
    }

    return records;
}

bool AlarmsRecordInterface::Update(const AlarmsRecord &rec) {

    auto ala = alarmsDB->alarms.GetByID(rec.dbID);
    if(ala.ID == 0){
        return false;
    }

    alarmsDB->alarms.Update(AlarmsTableRow{
        .ID=rec.dbID,
        .time = rec.time,
		.snooze = rec.snooze,
		.status = rec.status,
		.path = rec.path
    });

    return true;
}

bool AlarmsRecordInterface::RemoveByID(uint32_t id) {

    auto alarm = alarmsDB->alarms.GetByID(id);
    if(alarm.ID == 0){
        return false;
    }

//    ThreadRecordInterface threadInterface(alarmsDB,contactsDB);
//    auto threadRec = threadInterface.GetByID(sms.threadID);
//
//    // If thread not found
//    if(threadRec.dbID == 0){
//        if(smsDB->sms.RemoveByID(id) == false){
//            return false;
//        }
//
//        return false;
//    }
//
//    // If thread contains only one message remove it
//    if(threadRec.msgCount == 1){
//        threadInterface.RemoveByID(sms.threadID);
//    }
//    else{
//        // Update msg count
//        threadRec.msgCount--;
//        threadInterface.Update(threadRec);
//    }

    // Remove SMS
    if(alarmsDB->alarms.RemoveByID(id) == false){
        return false;
    }

    return true;
}

bool AlarmsRecordInterface::RemoveByField(AlarmsRecordField field, const char *str) {

    switch(field){
        case AlarmsRecordField ::Time:
            return alarmsDB->alarms.RemoveByField(AlarmsTableFields::Time,str);

        default:
            return false;

    }

}

AlarmsRecord AlarmsRecordInterface::GetByID(uint32_t id) {
    auto alarm = alarmsDB->alarms.GetByID(id);


//    ContactRecordInterface contactInterface(contactsDB);
//    auto contactRec = contactInterface.GetByID(sms.contactID);

    return AlarmsRecord{
        .ID = alarm.ID,
        .time = alarm.time,
		.snooze = alarm.snooze,
		.status = alarm.status,
		.path = alarm.path
    };

}


