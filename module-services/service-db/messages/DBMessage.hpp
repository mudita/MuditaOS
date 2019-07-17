
/*
 * @file DBMessage.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 06.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_DBMESSAGE_HPP
#define PUREPHONE_DBMESSAGE_HPP

#include <memory>
#include "Service/Message.hpp"
#include "MessageType.hpp"
#include "Interface/SettingsRecord.hpp"
#include "Interface/SMSRecord.hpp"
#include "Interface/ThreadRecord.hpp"
#include "Interface/ContactRecord.hpp"
#include "Interface/AlarmsRecord.hpp"


class DBMessage: public sys::DataMessage {
public:
    DBMessage( MessageType messageType ) : sys::DataMessage(static_cast<uint32_t>(messageType)),type(messageType) {};
    virtual ~DBMessage() {};

    MessageType type;

    uint32_t id;
    uint32_t offset;
    uint32_t limit;
};

class DBResponseMessage: public sys::ResponseMessage {
public:
    DBResponseMessage(uint32_t retCode,uint32_t count) : sys::ResponseMessage(),retCode(retCode),count(count) {};
    virtual ~DBResponseMessage() {};

    uint32_t retCode;
    uint32_t count;
};

class DBSettingsMessage : public DBMessage{
public:
    DBSettingsMessage(MessageType messageType,const SettingsRecord& rec = SettingsRecord{}): DBMessage(messageType),record(rec){

    }
    virtual ~DBSettingsMessage() {}

    SettingsRecord record;
};

class DBSettingsResponseMessage: public DBResponseMessage {
public:
    DBSettingsResponseMessage(const SettingsRecord& rec,uint32_t retCode=0,uint32_t count = 0) : DBResponseMessage(retCode,count),record(rec){};
    virtual ~DBSettingsResponseMessage() {};

    SettingsRecord record;
};

class DBSMSMessage : public DBMessage{
public:
    DBSMSMessage(MessageType messageType,const SMSRecord& rec = SMSRecord{}): DBMessage(messageType),record(rec){

    }
    virtual ~DBSMSMessage() {}

    SMSRecord record;
};

class DBSMSResponseMessage: public DBResponseMessage {
public:
    DBSMSResponseMessage(std::unique_ptr<std::vector<SMSRecord>> rec,uint32_t retCode=0,uint32_t count=0) : DBResponseMessage(retCode,count),records(std::move(rec)){};
    virtual ~DBSMSResponseMessage() {};

    std::unique_ptr<std::vector<SMSRecord>> records;
};

class DBThreadMessage : public DBMessage{
public:
    DBThreadMessage(MessageType messageType,const ThreadRecord& rec = ThreadRecord{}): DBMessage(messageType),record(rec){

    }
    virtual ~DBThreadMessage() {}

    ThreadRecord record;
};

class DBThreadResponseMessage: public DBResponseMessage {
public:
    DBThreadResponseMessage(std::unique_ptr<std::vector<ThreadRecord>> rec,uint32_t retCode=0,uint32_t count=0) : DBResponseMessage(retCode,count),records(std::move(rec)){};
    virtual ~DBThreadResponseMessage() {};

    std::unique_ptr<std::vector<ThreadRecord>> records;
};

class DBContactMessage : public DBMessage{
public:
    DBContactMessage(MessageType messageType,const ContactRecord& rec = ContactRecord{}): DBMessage(messageType),record(rec){

    }
    virtual ~DBContactMessage() {}

    ContactRecord record;
};

class DBContactResponseMessage: public DBResponseMessage {
public:
    DBContactResponseMessage(std::unique_ptr<std::vector<ContactRecord>> rec,uint32_t retCode=0,uint32_t  count=0) : DBResponseMessage(retCode,count),records(std::move(rec)){};
    virtual ~DBContactResponseMessage() {};

    std::unique_ptr<std::vector<ContactRecord>> records;
};

class DBAlarmMessage : public DBMessage{
public:
	DBAlarmMessage(MessageType messageType, const AlarmsRecord& rec = AlarmsRecord{}) : DBMessage(messageType), record(rec){

	}
	virtual ~DBAlarmMessage() {};
	AlarmsRecord record;
};

class DBAlarmResponseMessage : public DBResponseMessage{
public:
	DBAlarmResponseMessage(std::unique_ptr<std::vector<AlarmsRecord>> rec,uint32_t retCode=0,uint32_t  count=0) : DBResponseMessage(retCode,count),records(std::move(rec)){};
	virtual ~DBAlarmResponseMessage() {};

	std::unique_ptr<std::vector<AlarmsRecord>> records;
};
#endif //PUREPHONE_DBMESSAGE_HPP
