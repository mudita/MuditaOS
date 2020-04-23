#include "SMSTemplateRecord.hpp"

#include <log/log.hpp>

#include <cassert>

SMSTemplateRecord::SMSTemplateRecord(const SMSTemplateTableRow &w)
{
    ID                 = w.ID;
    text               = w.text;
    lastUsageTimestamp = w.lastUsageTimestamp;
}

SMSTemplateRecordInterface::SMSTemplateRecordInterface(SmsDB *smsDb) : smsDB(smsDb)
{}

SMSTemplateRecordInterface::~SMSTemplateRecordInterface()
{}

bool SMSTemplateRecordInterface::Add(const SMSTemplateRecord &rec)
{
    return smsDB->templates.Add(
        SMSTemplateTableRow{{.ID = rec.ID}, .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
}
uint32_t SMSTemplateRecordInterface::GetCount()
{
    return smsDB->templates.GetCount();
}

std::unique_ptr<std::vector<SMSTemplateRecord>> SMSTemplateRecordInterface::GetLimitOffsetByField(
    uint32_t offset, uint32_t limit, SMSTemplateRecordField field, const char *str)
{
    assert(0 && "need proper implementation");
    return GetLimitOffset(offset, limit);
}

std::unique_ptr<std::vector<SMSTemplateRecord>> SMSTemplateRecordInterface::GetLimitOffset(uint32_t offset,
                                                                                           uint32_t limit)
{
    auto templates = smsDB->templates.GetLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<SMSTemplateRecord>>(templates.begin(), templates.end());

    return records;
}

bool SMSTemplateRecordInterface::Update(const SMSTemplateRecord &rec)
{
    auto templ = smsDB->templates.GetByID(rec.ID);
    if (templ.ID == DB_ID_NONE) {
        return false;
    }

    return smsDB->templates.Update(
        SMSTemplateTableRow{{.ID = rec.ID}, .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
}

bool SMSTemplateRecordInterface::RemoveByID(uint32_t id)
{

    auto templ = smsDB->templates.GetByID(id);
    if (templ.ID == 0) {
        return false;
    }

    return smsDB->templates.RemoveByID(id);
}

bool SMSTemplateRecordInterface::RemoveByField(SMSTemplateRecordField field, const char *str)
{
    assert(0 && "need implementation");
    return false;
}

SMSTemplateRecord SMSTemplateRecordInterface::GetByID(uint32_t id)
{
    auto templ = smsDB->templates.GetByID(id);

    return SMSTemplateRecord{templ};
}
