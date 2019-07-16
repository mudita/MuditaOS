/*
 * AlarmsRecord.hpp
 *
 *  Created on: 15 lip 2019
 *      Author: kuba
 */

#ifndef MODULE_DB_INTERFACE_ALARMSRECORD_HPP_
#define MODULE_DB_INTERFACE_ALARMSRECORD_HPP_


#include "Record.hpp"
#include <stdint.h>
#include "../Databases/AlarmsDB.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

struct AlarmsRecord{
	uint32_t ID;
	uint32_t time;
	uint32_t snooze;
	uint32_t status;
	UTF8    path;

};

enum class AlarmsRecordField{
    Time,
    Snooze,
    Status,
	Path
};

class AlarmsRecordInterface : public RecordInterface<AlarmsRecord,AlarmsRecordField > {
public:

    AlarmsRecordInterface(AlarmsDB* alarmsDb);
    ~AlarmsRecordInterface();

    bool Add(const AlarmsRecord& rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByField(AlarmsRecordField field,const char* str) override final;
    bool Update(const AlarmsRecord& rec) override final;
    AlarmsRecord GetByID(uint32_t id) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<AlarmsRecord>> GetLimitOffset(uint32_t offset,uint32_t limit) override final;

    std::unique_ptr<std::vector<AlarmsRecord>> GetLimitOffsetByField(uint32_t offset,uint32_t limit,AlarmsRecordField field, const char* str) override final;

private:
    const uint32_t snippetLength = 45;
    AlarmsDB* alarmsDB;
};


#endif /* MODULE_DB_INTERFACE_ALARMSRECORD_HPP_ */
