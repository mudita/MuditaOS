/*
 * @file CalllogRecord.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB Record
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CalllogRecord.hpp"
#include "../Tables/CalllogTable.hpp"

CalllogRecordInterface::CalllogRecordInterface(CalllogDB* calllogDb): calllogDB(calllogDb) {
}

CalllogRecordInterface::~CalllogRecordInterface() {
}

bool CalllogRecordInterface::Add(const CalllogRecord &rec)
{
    return calllogDB->calls.Add(CalllogTableRow{.id = rec.id, // this is only to remove warning
                                                .number = rec.number,
                                                .presentation = rec.presentation,
                                                .date = rec.date,
                                                .duration = rec.duration,
                                                .type = rec.type,
                                                .name = rec.name,
                                                .contactId = rec.contactId});
}

uint32_t CalllogRecordInterface::GetLastID()
{
    return calllogDB->GetLastInsertRowID();
}

std::unique_ptr<std::vector<CalllogRecord>> CalllogRecordInterface::GetLimitOffsetByField(uint32_t offset, uint32_t limit,
                                                                                  CalllogRecordField field,
                                                                                  const char *str) {
	// TODO: alek: need proper implementation
	return GetLimitOffset(offset,limit);
}


std::unique_ptr<std::vector<CalllogRecord>> CalllogRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit) {
    auto calls = calllogDB->calls.GetLimitOffset(offset,limit);

    auto records = std::make_unique<std::vector<CalllogRecord>>();

	CalllogRecordInterface callsInterface(calllogDB);
        for(const auto &rec : calls){

            records->push_back({
                .id=rec.id,
                .number=rec.number,
                .presentation=rec.presentation,
                .date=rec.date,
			    .duration=rec.duration,
                .type=rec.type,
                .name=rec.name,
                .contactId=rec.contactId
            });
        }

        return records;
}

bool CalllogRecordInterface::Update(const CalllogRecord &rec) {

    auto call = calllogDB->calls.GetByID(rec.id);
    if(call.id == 0){
        return false;
    }

    calllogDB->calls.Update(CalllogTableRow{
        .id=rec.id, 
        .number=rec.number,
        .presentation=rec.presentation,
        .date=rec.date,
        .duration=rec.duration,
        .type=rec.type,
        .name=rec.name,
        .contactId=rec.contactId
    });

    return true;
}

bool CalllogRecordInterface::RemoveByID(uint32_t id) {

    auto call = calllogDB->calls.GetByID(id);
    if(call.id == 0){
        return false;
    }

    if(calllogDB->calls.RemoveByID(id) == false){
        return false;
    }

    return true;
}

bool CalllogRecordInterface::RemoveByField(CalllogRecordField field, const char *str) {

    switch(field){
        case CalllogRecordField::DATE:
            return calllogDB->calls.RemoveByField(CalllogTableFields::DATE,str);
        case CalllogRecordField::TYPE:
            return calllogDB->calls.RemoveByField(CalllogTableFields::TYPE,str);    
        default:
            return false;

    }
}

CalllogRecord CalllogRecordInterface::GetByID(uint32_t id) {
    auto rec = calllogDB->calls.GetByID(id);

    return CalllogRecord{
        .id=rec.id, // this is only to remove warning
        .number=rec.number,
        .presentation=rec.presentation,
        .date=rec.date,
        .duration=rec.duration,
        .type=rec.type,
        .name=rec.name,
        .contactId=rec.contactId
    };
}

uint32_t CalllogRecordInterface::GetCount() {
    return calllogDB->calls.GetCount();
}



