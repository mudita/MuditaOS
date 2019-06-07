
/*
 * @file SMSRecord.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "SMSRecord.hpp"
#include "ContactRecord.hpp"
#include "ThreadRecord.hpp"

SMSRecordInterface::SMSRecordInterface() {
    smsDB = std::make_unique<SmsDB>();
}

SMSRecordInterface::~SMSRecordInterface() {

}

bool SMSRecordInterface::Add(const SMSRecord &rec) {

    uint32_t contactID = 0;

    ContactRecordInterface contactInterface;
    auto contactRec = contactInterface.GetLimitOffsetByField(0, 1, ContactRecordField::NumberE164, rec.number.c_str());

    // Contact not found, create one
    if (contactRec->size() == 0) {
        contactInterface.Add(ContactRecord{
                .numberUser=rec.number,
                .numberE164=rec.number,
                .contactType=ContactType::MESSAGE,
        });

        contactRec = contactInterface.GetLimitOffsetByField(0, 1, ContactRecordField::NumberE164,
                                                                 rec.number.c_str());
    }
    contactID = (*contactRec)[0].dbID;



    // Search for a thread with specified contactID
    uint32_t threadID =0;
    ThreadRecordInterface threadInterface;
    auto threadRec = threadInterface.GetLimitOffsetByField(0, 1, ThreadRecordField::ContactID,
                                                           std::to_string(contactID).c_str());

    // Thread not found, create one
    if (threadRec->size() == 0) {

        threadInterface.Add(ThreadRecord{
                .contactID=contactID,
        });

        threadRec = threadInterface.GetLimitOffsetByField(0,1,ThreadRecordField::ContactID,std::to_string(contactID).c_str());

    }
    threadID = (*threadRec)[0].dbID;

    // Create SMS
    smsDB->sms.Add(SMSTableRow{
            .threadID = threadID,
            .contactID=contactID,
            .date=rec.date,
            .dateSent=rec.dateSent,
            .errorCode=rec.errorCode,
            .body=rec.body,
            .isRead=rec.isRead,
            .type=rec.type

    });

    //TODO: error check

    // Update thread
    auto thread = (*threadRec)[0];
    thread.snippet = rec.body.substr(0,rec.body.length() >= snippetLength ? snippetLength : rec.body.length());
    thread.date = rec.date;
    thread.type = rec.type;
    thread.msgCount++;
    if(rec.type == SMSType::INBOX){
        thread.msgRead++;
    }

    threadInterface.Update(thread);


    return true;
}

uint32_t SMSRecordInterface::GetCount() {
    return smsDB->sms.GetCount();
}

std::unique_ptr<std::vector<SMSRecord>> SMSRecordInterface::GetLimitOffsetByField(uint32_t offset, uint32_t limit,
                                                                                  SMSRecordField field,
                                                                                  const char *str) {
    auto records = std::make_unique<std::vector<SMSRecord>>();

    std::vector<SMSTableRow> smses;

    switch(field){
        case SMSRecordField ::ContactID:
            smses = smsDB->sms.GetLimitOffsetByField(offset,limit,SMSTableFields::ContactID,str);
            break;

        case SMSRecordField ::ThreadID:
            smses = smsDB->sms.GetLimitOffsetByField(offset,limit,SMSTableFields::ThreadID,str);
            break;

        default:
            return records;
    }


    ContactRecordInterface contactInterface;
    for(const auto &w : smses){

        auto contactRec = contactInterface.GetByID(w.contactID);

        records->push_back({
                                   .dbID=w.ID,
                                   .date=w.date,
                                   .dateSent=w.dateSent,
                                   .errorCode=w.errorCode,
                                   .number=contactRec.numberE164,// TODO: or numberUser?
                                   .body=w.body,
                                   .isRead=w.isRead,
                                   .type=w.type,
                                   .threadID=w.threadID,
                                   .contactID=w.contactID

                           });
    }

    return records;
}


std::unique_ptr<std::vector<SMSRecord>> SMSRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto smses = smsDB->sms.GetLimitOffset(offset,limit);

    auto records = std::make_unique<std::vector<SMSRecord>>();

    ContactRecordInterface contactInterface;
    for(const auto &w : smses){

        auto contactRec = contactInterface.GetByID(w.contactID);

        records->push_back({
           .dbID=w.ID,
           .date=w.date,
           .dateSent=w.dateSent,
           .errorCode=w.errorCode,
           .number=contactRec.numberE164,// TODO: or numberUser?
           .body=w.body,
           .isRead=w.isRead,
           .type=w.type,
           .threadID=w.threadID,
           .contactID=w.contactID

        });
    }

    return records;
}

bool SMSRecordInterface::Update(const SMSRecord &rec) {

    auto sms = smsDB->sms.GetByID(rec.dbID);
    if(sms.ID == 0){
        return false;
    }

    smsDB->sms.Update(SMSTableRow{
        .ID=rec.dbID,
        .threadID=sms.threadID,
        .contactID=sms.contactID,
        .date=rec.date,
        .dateSent=rec.dateSent,
        .errorCode=rec.errorCode,
        .body=rec.body,
        .isRead=rec.isRead,
        .type=rec.type
    });

    // Update messages read count if necessary
    if(!sms.isRead && rec.isRead){
        ThreadRecordInterface threadInterface;
        auto threadRec =   threadInterface.GetByID(sms.threadID);
        threadRec.msgRead--;
        threadInterface.Update(threadRec);

    }

    return true;
}

bool SMSRecordInterface::RemoveByID(uint32_t id) {

    auto sms = smsDB->sms.GetByID(id);
    if(sms.ID == 0){
        return false;
    }

    ThreadRecordInterface threadInterface;
    auto threadRec = threadInterface.GetByID(sms.threadID);

    // If thread not found
    if(threadRec.dbID == 0){
        if(smsDB->sms.RemoveByID(id) == false){
            return false;
        }

        return false;
    }

    // If thread contains only one message remove it
    if(threadRec.msgCount == 1){
        threadInterface.RemoveByID(sms.threadID);
    }
    else{
        // Update msg count
        threadRec.msgCount--;
        threadInterface.Update(threadRec);
    }

    // Remove SMS
    if(smsDB->sms.RemoveByID(id) == false){
        return false;
    }

    return true;
}

bool SMSRecordInterface::RemoveByField(SMSRecordField field, const char *str) {

    switch(field){
        case SMSRecordField ::ContactID:
            return smsDB->sms.RemoveByField(SMSTableFields::ContactID,str);

        case SMSRecordField ::ThreadID:
            return smsDB->sms.RemoveByField(SMSTableFields::ThreadID,str);

        case SMSRecordField ::Number:
            return false;

        default:
            return false;

    }

}

SMSRecord SMSRecordInterface::GetByID(uint32_t id) {
    auto sms = smsDB->sms.GetByID(id);


    ContactRecordInterface contactInterface;
    auto contactRec = contactInterface.GetByID(sms.contactID);

    return SMSRecord{
        .dbID=sms.ID,
        .date=sms.date,
        .dateSent=sms.dateSent,
        .errorCode=sms.errorCode,
        .number=contactRec.numberE164,// TODO: or numberUser?
        .body=sms.body,
        .isRead=sms.isRead,
        .type=sms.type,
        .threadID=sms.threadID,
        .contactID=sms.contactID

    };

}


