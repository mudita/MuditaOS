
/*
 * @file ThreadRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_THREADRECORD_HPP
#define PUREPHONE_THREADRECORD_HPP

#include "Record.hpp"
#include <stdint.h>
#include "utf8/UTF8.hpp"
#include "../Databases/SmsDB.hpp"
#include "../Common/Common.hpp"


struct ThreadRecord{
    uint32_t dbID;
    uint32_t date;
    uint32_t msgCount;
    uint32_t msgRead;
    UTF8 snippet;
    SMSType type;
    uint32_t contactID;
};

enum class ThreadRecordField{
    ContactID,
};

class ThreadRecordInterface : public RecordInterface<ThreadRecord,ThreadRecordField>{
public:

    ThreadRecordInterface();
    ~ThreadRecordInterface();

    bool Add(const ThreadRecord& rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool Update(const ThreadRecord& rec) override final;
    ThreadRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffset(uint32_t offset,uint32_t limit) override final;

    std::unique_ptr<std::vector<ThreadRecord>> GetLimitOffsetByField(uint32_t offset,uint32_t limit,ThreadRecordField field, const char* str) override final;
private:
    std::unique_ptr<SmsDB> smsDB;

};


#endif //PUREPHONE_THREADRECORD_HPP
