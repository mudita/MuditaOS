
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

        if(settingsResponse->retCode == true){
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


bool DBServiceAPI::SMSAdd(sys::Service *serv, const SMSRecord &rec) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSAdd,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::SMSRemove(sys::Service *serv, uint32_t id) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSRemove);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::SMSUpdate(sys::Service *serv, const SMSRecord &rec) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSUpdate,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

std::unique_ptr<std::vector<SMSRecord>> DBServiceAPI::SMSGetLimitOffset(sys::Service *serv,uint32_t offset, uint32_t limit) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSGetSMSLimitOffset);
    msg->offset = offset;
    msg->limit = limit;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return std::move(smsResponse->records);
    }
    else{
        return std::make_unique<std::vector<SMSRecord>>();
    }
}

std::unique_ptr<std::vector<SMSRecord>> DBServiceAPI::SMSGetLimitOffsetByThreadID(sys::Service *serv, uint32_t offset,
                                                                                  uint32_t limit, uint32_t id) {
    std::shared_ptr<DBSMSMessage> msg = std::make_shared<DBSMSMessage>(MessageType::DBSMSGetSMSLimitOffsetByThreadID);
    msg->offset = offset;
    msg->limit = limit;
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBSMSResponseMessage* smsResponse = reinterpret_cast<DBSMSResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (smsResponse->retCode == true)){
        return std::move(smsResponse->records);
    }
    else{
        return std::make_unique<std::vector<SMSRecord>>();
    }
}

ThreadRecord DBServiceAPI::ThreadGet(sys::Service *serv,uint32_t id) {
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGet);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
        return std::move((*threadResponse->records)[0]);
    }
    else{
        return ThreadRecord{};
    }
}

bool DBServiceAPI::ThreadRemove(sys::Service *serv, uint32_t id) {
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadRemove);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::ThreadGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit) {
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGetLimitOffset);
    msg->offset = offset;
    msg->limit = limit;

    sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv);
//    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
//    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
//        return std::move(threadResponse->records);
//    }
//    else{
//        return std::make_unique<std::vector<ThreadRecord>>();
//    }
    return true;
}

uint32_t DBServiceAPI::ThreadGetCount(sys::Service * serv)
{
    std::shared_ptr<DBThreadMessage> msg = std::make_shared<DBThreadMessage>(MessageType::DBThreadGetCount);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBThreadResponseMessage* threadResponse = reinterpret_cast<DBThreadResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (threadResponse->retCode == true)){
        return threadResponse->count;
    }
    else{
        return false;
    }
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByName(sys::Service *serv, UTF8 primaryName, UTF8 alternativeName ) {

	ContactRecord rec;
	rec.primaryName = primaryName;
	rec.alternativeName = alternativeName;

    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByName,rec );

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
		return std::move(contactResponse->records);
	}
	else{
		return std::make_unique<std::vector<ContactRecord>>();
	}
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetBySpeeddial(sys::Service *serv, uint8_t speeddial ) {
	ContactRecord rec;
	rec.speeddial = speeddial;

	std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetBySpeedDial,rec );

	auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
	DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
	if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
		return std::move(contactResponse->records);
	}
	else{
		return std::make_unique<std::vector<ContactRecord>>();
	}
}

std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetByPhoneNumber(sys::Service *serv, UTF8 phoneNumber) {
	ContactRecord rec;
	rec.number = phoneNumber;

	std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetByNumber,rec );

	auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
	DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
	if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
		return std::move(contactResponse->records);
	}
	else{
		return std::make_unique<std::vector<ContactRecord>>();
	}
}

bool DBServiceAPI::verifyContact( sys::Service* serv,const ContactRecord& rec,
		ContactRecord& errName, ContactRecord& errPhone1, ContactRecord& errPhone2, ContactRecord& speedDial ) {

	//if true it means that contact passed verification
	bool verified = true;

	//request contact with specified primary and alternative name.
	auto retName = ContactGetByName( serv, rec.primaryName, rec.alternativeName );
	if( retName->size() != 0 ) {
		verified = false;
		errName = retName->operator [](0);
		LOG_WARN("Name verification failed for [%s %s] owner ID: %d [%s %s]",
			rec.primaryName.c_str(), rec.alternativeName.c_str());
	}
	else {
		LOG_INFO("Name verification passed for [%s %s]", rec.primaryName.c_str(), rec.alternativeName.c_str());
	}

	//request contact by speed dial
	auto retSpeedDial = ContactGetBySpeeddial( serv, rec.speeddial );
	if( retSpeedDial->size() != 0 ) {
		verified = false;
		LOG_WARN("Speed dial verification failed for number: [%d] owner ID: %d [%s %s]",
			rec.speeddial,
			retSpeedDial->operator [](0).dbID,
			retSpeedDial->operator [](0).primaryName.c_str(),
			retSpeedDial->operator [](0).alternativeName.c_str());
		speedDial = retSpeedDial->operator [](0);
	}
	else {
		LOG_INFO("Speed dial verification passed for number: [%d]", rec.speeddial);
	}

	//request contact by speed dial
	if( rec.numbers.size() > 0 ) {
		auto retPhone1 = ContactGetByPhoneNumber( serv, rec.numbers[0].numberE164 );
		if( retPhone1->size() != 0 ) {
			verified = false;
			LOG_WARN("Phone number 1 verification failed for number: [%s] owner: ID: %d [%s %s]",
				rec.numbers[0].numberE164.c_str(),
				retPhone1->operator [](0).dbID,
				retPhone1->operator [](0).primaryName.c_str(),
				retPhone1->operator [](0).alternativeName.c_str());
			errPhone1 = retPhone1->operator [](0);
		}
		else {
			LOG_INFO("Phone number 1 verification passed for number: [%s]", rec.numbers[0].numberE164.c_str());
		}
	}

	if( rec.numbers.size() > 1 ) {
		auto retPhone2 = ContactGetByPhoneNumber( serv, rec.numbers[0].numberE164 );
		if( retPhone2->size() != 0 ) {
			verified = false;
			LOG_WARN("Phone number 1 verification failed for number: [%s] owner: ID: %d [%s %s]",
				rec.numbers[0].numberE164.c_str(),
				retPhone2->operator [](0).dbID,
				retPhone2->operator [](0).primaryName.c_str(),
				retPhone2->operator [](0).alternativeName.c_str());
			errPhone2 = retPhone2->operator [](0);
		}
		else {
			LOG_INFO("Phone number 2 verification passed for number: [%s]", rec.numbers[1].numberE164.c_str());
		}
	}

	return verified;
}

bool DBServiceAPI::ContactAdd(sys::Service *serv, const ContactRecord &rec) {
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactAdd,rec);
    msg->record.contactType = ContactType::USER;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::ContactRemove(sys::Service *serv, uint32_t id) {
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactRemove);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::ContactUpdate(sys::Service *serv, const ContactRecord &rec) {
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactUpdate,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

uint32_t DBServiceAPI::ContactGetCount(sys::Service *serv, bool favourites ) {
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetCount, ContactRecord{}, favourites);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
        return contactResponse->count;
    }
    else{
        return false;
    }
}

//std::unique_ptr<std::vector<ContactRecord>> DBServiceAPI::ContactGetLimitOffset(sys::Service *serv, uint32_t offset,
bool DBServiceAPI::ContactGetLimitOffset(sys::Service *serv, uint32_t offset,
		uint32_t limit, bool favourites ) {
    std::shared_ptr<DBContactMessage> msg = std::make_shared<DBContactMessage>(MessageType::DBContactGetLimitOffset, ContactRecord{}, favourites);
    msg->offset = offset;
    msg->limit = limit;

//    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
//    DBContactResponseMessage* contactResponse = reinterpret_cast<DBContactResponseMessage*>(ret.second.get());
//    contactResponse->offset = msg->offset;
//    contactResponse->limit = msg->limit;
//    contactResponse->favourite = favourites;
//
//    if((ret.first == sys::ReturnCodes::Success) && (contactResponse->retCode == true)){
//        return std::move(contactResponse->records);
//    }
//    else{
//        return std::make_unique<std::vector<ContactRecord>>();
//    }

    sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv);
        return true;
}

bool DBServiceAPI::AlarmAdd(sys::Service *serv, const AlarmsRecord &rec) {
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmAdd,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBAlarmResponseMessage* alarmResponse = reinterpret_cast<DBAlarmResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::AlarmRemove(sys::Service *serv, uint32_t id) {
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmRemove);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBAlarmResponseMessage* alarmResponse = reinterpret_cast<DBAlarmResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::AlarmUpdate(sys::Service *serv, const AlarmsRecord &rec) {
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmUpdate,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBAlarmResponseMessage* alarmResponse = reinterpret_cast<DBAlarmResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

uint32_t DBServiceAPI::AlarmGetCount(sys::Service *serv) {
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmGetCount);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBAlarmResponseMessage* alarmResponse = reinterpret_cast<DBAlarmResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)){
        return alarmResponse->count;
    }
    else{
        return false;
    }
}

std::unique_ptr<std::vector<AlarmsRecord>> DBServiceAPI::AlarmGetLimitOffset(sys::Service *serv, uint32_t offset,
                                                                                uint32_t limit) {
    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmGetLimitOffset);
    msg->offset = offset;
    msg->limit = limit;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBAlarmResponseMessage* alarmResponse = reinterpret_cast<DBAlarmResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)){
        return std::move(alarmResponse->records);
    }
    else{
        return std::make_unique<std::vector<AlarmsRecord>>();
    }
}

AlarmsRecord DBServiceAPI::AlarmGetNext(sys::Service *serv, time_t time) {

    std::shared_ptr<DBAlarmMessage> msg = std::make_shared<DBAlarmMessage>(MessageType::DBAlarmGetNext);
    msg->time = time;
    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBAlarmResponseMessage* alarmResponse = reinterpret_cast<DBAlarmResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (alarmResponse->retCode == true)){
    	return std::move((*alarmResponse->records)[0]);
    }
    else{
        return AlarmsRecord{};
    }
}

bool DBServiceAPI::NotesAdd(sys::Service *serv, const NotesRecord &rec) {
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesAdd,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBNotesResponseMessage* notesResponse = reinterpret_cast<DBNotesResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::NotesRemove(sys::Service *serv, uint32_t id) {
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesRemove);
    msg->id = id;

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBNotesResponseMessage* notesResponse = reinterpret_cast<DBNotesResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::NotesUpdate(sys::Service *serv, const NotesRecord &rec) {
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesUpdate,rec);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBNotesResponseMessage* notesResponse = reinterpret_cast<DBNotesResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)){
        return true;
    }
    else{
        return false;
    }
}

uint32_t DBServiceAPI::NotesGetCount(sys::Service *serv) {
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesGetCount);

    auto ret = sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv,5000);
    DBNotesResponseMessage* notesResponse = reinterpret_cast<DBNotesResponseMessage*>(ret.second.get());
    if((ret.first == sys::ReturnCodes::Success) && (notesResponse->retCode == true)){
        return notesResponse->count;
    }
    else{
        return false;
    }
}

bool DBServiceAPI::NotesGetLimitOffset(sys::Service *serv, uint32_t offset, uint32_t limit) {
    std::shared_ptr<DBNotesMessage> msg = std::make_shared<DBNotesMessage>(MessageType::DBNotesGetLimitOffset);
    msg->offset = offset;
    msg->limit = limit;

    sys::Bus::SendUnicast(msg,ServiceDB::serviceName,serv);
    return true;
}

