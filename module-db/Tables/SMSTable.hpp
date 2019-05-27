
/*
 * @file SMSTable.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 27.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_SMSTABLE_HPP
#define PUREPHONE_SMSTABLE_HPP

#include "Table.hpp"
#include "Common/SMSRecord.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"

/**
 * Types of sms message
 */
enum class SMSType {
    DRAFT = 0x01,
    FAILED = 0x02,
    INBOX = 0x04,
    OUTBOX = 0x08,
    QUEUED = 0x10,
    ALL = 0xFF
};

struct SMSTableRow{
    uint32_t ID;
    uint32_t threadID;
    uint32_t contactID;
    uint32_t date;
    uint32_t dateSent;
    uint32_t errorCode;
    UTF8    body;
    bool    isRead;
    SMSType type;
};

class SMSTable : public Table<SMSTableRow> {
public:

    SMSTable();
    ~SMSTable();

    bool Add(SMSTableRow entry) override final;
    bool Remove(SMSTableRow entry) override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(SMSTableRow entry) override final;
    SMSTableRow GetByID(uint32_t id) override final;
    std::vector<SMSTableRow> GetLimitOffset(uint32_t offset,uint32_t limit);

    uint32_t GetCount() override final;
private:

    const char* createTableQuery =
            "CREATE TABLE IF NOT EXISTS sms("
            "_id INTEGER PRIMARY KEY,"
            "thread_id INTEGER,"
            "contact_id INTEGER,"
            "date INTEGER,"
            "date_send INTEGER,"
            "error_code INTEGER,"
            "body TEXT NOT_NULL,"
            "read INTEGER,"
            "type INTEGER,"
            "FOREIGN KEY(thread_id) REFERENCES threads(_id) ON DELETE CASCADE );";


    Database db;

    bool isInitialized;
};


#endif //PUREPHONE_SMSTABLE_HPP
