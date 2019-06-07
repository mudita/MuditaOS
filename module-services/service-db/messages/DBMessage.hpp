
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


class DBMessage: public sys::DataMessage {
public:
    DBMessage( MessageType messageType ) : sys::DataMessage(static_cast<uint32_t>(messageType)),type(messageType) {};
    virtual ~DBMessage() {};

    MessageType type;
};

class DBResponseMessage: public sys::ResponseMessage {
public:
    DBResponseMessage() : sys::ResponseMessage() {};
    virtual ~DBResponseMessage() {};

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
    DBSettingsResponseMessage(const SettingsRecord& rec,uint32_t retCode=0) : DBResponseMessage(),record(rec),retCode(retCode) {};
    virtual ~DBSettingsResponseMessage() {};

    SettingsRecord record;
    uint32_t retCode;
};

class DBSMSMessage : public DBMessage{
public:
    DBSMSMessage(MessageType messageType,const SMSRecord& rec = SMSRecord{}): DBMessage(messageType),record(rec){

    }
    virtual ~DBSMSMessage() {}

    uint32_t id;
    uint32_t offset;
    uint32_t limit;
    SMSRecord record;
};

class DBSMSResponseMessage: public DBResponseMessage {
public:
    DBSMSResponseMessage(std::unique_ptr<std::vector<SMSRecord>> rec,uint32_t retCode=0) : DBResponseMessage(),records(std::move(rec)),retCode(retCode) {};
    virtual ~DBSMSResponseMessage() {};

    std::unique_ptr<std::vector<SMSRecord>> records;
    uint32_t retCode;
};


#endif //PUREPHONE_DBMESSAGE_HPP
