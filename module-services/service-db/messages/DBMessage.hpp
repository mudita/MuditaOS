
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
#include "Interface/NotesRecord.hpp"
#include "Interface/CalllogRecord.hpp"


class DBMessage: public sys::DataMessage {
public:
    DBMessage( MessageType messageType ) : sys::DataMessage(static_cast<uint32_t>(messageType)),type(messageType) {};
    virtual ~DBMessage() {};

    MessageType type;

    uint32_t id = 0;
    uint32_t offset = 0;
    uint32_t limit = 0;
};

class DBResponseMessage: public sys::ResponseMessage {
public:
    DBResponseMessage(uint32_t retCode,uint32_t count,uint32_t responseTo) : sys::ResponseMessage(sys::ReturnCodes::Success,responseTo),retCode(retCode),count(count) {};
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
    DBSettingsResponseMessage(const SettingsRecord& rec,uint32_t retCode=0,uint32_t count = 0,uint32_t respTo=0) : DBResponseMessage(retCode,count,respTo),record(rec){};
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
    DBSMSResponseMessage(std::unique_ptr<std::vector<SMSRecord>> rec,uint32_t retCode=0,uint32_t count=0,uint32_t respTo=0) : DBResponseMessage(retCode,count,respTo),records(std::move(rec)){};
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
    DBThreadResponseMessage(std::unique_ptr<std::vector<ThreadRecord>> rec,uint32_t retCode=0, uint32_t limit=0,uint32_t offset=0,uint32_t count=0,uint32_t respTo=0) : DBResponseMessage(retCode,count,respTo),
	records(std::move(rec)),
	limit( limit ),
	offset( offset )
	{
    	this->count = count;
	};
    virtual ~DBThreadResponseMessage() {};

    std::unique_ptr<std::vector<ThreadRecord>> records;
    uint32_t limit = 0;
	uint32_t offset = 0;
};

class DBContactMessage : public DBMessage{
public:
    DBContactMessage(MessageType messageType,const ContactRecord& rec = ContactRecord{}, bool favourite = false ):
    	DBMessage(messageType),record(rec), favourite{favourite}{

    }
    virtual ~DBContactMessage() {}

    ContactRecord record;
    bool favourite = false;
};

class DBContactResponseMessage: public DBResponseMessage {
public:
    DBContactResponseMessage(std::unique_ptr<std::vector<ContactRecord>> rec, uint32_t retCode=0,
		uint32_t limit=0,uint32_t offset=0, bool favourite = false,
		uint32_t  count=0,uint32_t respTo=0 ) :
    	DBResponseMessage(retCode,count,respTo),
		records(std::move(rec)), favourite{favourite}, limit{limit}, offset{offset}{};
    virtual ~DBContactResponseMessage() {};

    std::unique_ptr<std::vector<ContactRecord>> records;
    bool favourite = false;
    uint32_t limit = 0;
	uint32_t offset = 0;
};

class DBAlarmMessage : public DBMessage{
public:
	DBAlarmMessage(MessageType messageType, const AlarmsRecord& rec = AlarmsRecord{}) : DBMessage(messageType), record(rec){

	}
	virtual ~DBAlarmMessage() {};
	AlarmsRecord record;
	time_t time = 0;
};

class DBAlarmResponseMessage : public DBResponseMessage{
public:
	DBAlarmResponseMessage(std::unique_ptr<std::vector<AlarmsRecord>> rec,uint32_t retCode=0,uint32_t  count=0,uint32_t respTo=0) : DBResponseMessage(retCode,count,respTo),records(std::move(rec)){};
	virtual ~DBAlarmResponseMessage() {};

	std::unique_ptr<std::vector<AlarmsRecord>> records;
};

class DBNotesMessage : public DBMessage{
public:
    DBNotesMessage(MessageType messageType,const NotesRecord& rec = NotesRecord{}): DBMessage(messageType),record(rec){

    }
    virtual ~DBNotesMessage() {}

    NotesRecord record;
};

class DBNotesResponseMessage: public DBResponseMessage {
public:
    DBNotesResponseMessage(std::unique_ptr<std::vector<NotesRecord>> rec,uint32_t retCode=0,uint32_t limit=0,uint32_t offset=0, uint32_t count=0,uint32_t respTo=0) :
    	DBResponseMessage(retCode,count,respTo),records(std::move(rec)),
		limit( limit ),
		offset( offset )
	{
    	this->count = count;
	};
    virtual ~DBNotesResponseMessage() {};

    std::unique_ptr<std::vector<NotesRecord>> records;
    uint32_t limit = 0;
	uint32_t offset = 0;
};


class DBCalllogMessage : public DBMessage{
public:
    DBCalllogMessage(MessageType messageType,const CalllogRecord& rec = CalllogRecord{}): DBMessage(messageType),record(rec){

    }
    virtual ~DBCalllogMessage() {}

    CalllogRecord record;
};

class DBCalllogResponseMessage: public DBResponseMessage {
public:
    DBCalllogResponseMessage(std::unique_ptr<std::vector<CalllogRecord>> rec,uint32_t retCode=0,uint32_t limit=0,uint32_t offset=0, uint32_t count=0,uint32_t respTo=0) :
    	DBResponseMessage(retCode,count,respTo),records(std::move(rec)),
		limit( limit ),
		offset( offset )
	{
    	this->count = count;
	};
    virtual ~DBCalllogResponseMessage() {};

    std::unique_ptr<std::vector<CalllogRecord>> records;
    uint32_t limit = 0;
	uint32_t offset = 0;
};

#endif //PUREPHONE_DBMESSAGE_HPP
