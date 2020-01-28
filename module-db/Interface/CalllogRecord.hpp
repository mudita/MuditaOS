/*
 * @file CalllogRecord.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB Record
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_DB_INTERFACE_CALLLOGRECORD_HPP_
#define MODULE_DB_INTERFACE_CALLLOGRECORD_HPP_

#include "Record.hpp"
#include <stdint.h>
#include "../Databases/CalllogDB.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

struct CalllogRecord{
    uint32_t    id;
    UTF8        number;
	PresentationType presentation;
    time_t      date;
    time_t duration;
    CallType    type;
    UTF8        name;
	UTF8        contactId;

    static std::string to_string(const CalllogRecord &rec);
};

enum class CalllogRecordField{
    DATE,
	TYPE,
 };

/*
 *
 */
class CalllogRecordInterface : public RecordInterface<CalllogRecord,CalllogRecordField > {
public:

    CalllogRecordInterface(CalllogDB* CalllogDb);
    virtual ~CalllogRecordInterface();

    bool Add(const CalllogRecord& rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(CalllogRecordField field,const char* str) override final;
    bool Update(const CalllogRecord& rec) override final;
    CalllogRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<CalllogRecord>> GetLimitOffset(uint32_t offset,uint32_t limit) override final;

    std::unique_ptr<std::vector<CalllogRecord>> GetLimitOffsetByField(uint32_t offset,uint32_t limit,CalllogRecordField field, const char* str) override final;

    uint32_t GetLastID();

  private:
    const uint32_t snippetLength = 60;
    CalllogDB* calllogDB;
};

#endif /* MODULE_DB_INTERFACE_CALLLOGRECORD_HPP_ */
