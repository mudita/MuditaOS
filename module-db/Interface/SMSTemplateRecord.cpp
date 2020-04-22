#include "SMSTemplateRecord.hpp"

#include <log/log.hpp>

#include <cassert>

SMSTemplatesRecord::SMSTemplatesRecord(const SMSTemplatesTableRow &w)
{
    ID                 = w.ID;
    text               = w.text;
    lastUsageTimestamp = w.lastUsageTimestamp;
}

SMSTemplatesRecordInterface::SMSTemplatesRecordInterface(SmsDB *smsDb) : smsDB(smsDb)
{}

SMSTemplatesRecordInterface::~SMSTemplatesRecordInterface()
{}

bool SMSTemplatesRecordInterface::Add(const SMSTemplatesRecord &rec)
{
    return smsDB->templates.Add(
        SMSTemplatesTableRow{{.ID = rec.ID}, .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
}
uint32_t SMSTemplatesRecordInterface::GetCount()
{
    return smsDB->templates.GetCount();
}

std::unique_ptr<std::vector<SMSTemplatesRecord>> SMSTemplatesRecordInterface::GetLimitOffsetByField(
    uint32_t offset, uint32_t limit, SMSTemplatesRecordField field, const char *str)
{
    assert(0 && "need proper implementation");
    return GetLimitOffset(offset, limit);
}

std::unique_ptr<std::vector<SMSTemplatesRecord>> SMSTemplatesRecordInterface::GetLimitOffset(uint32_t offset,
                                                                                             uint32_t limit)
{
    auto templates = smsDB->templates.GetLimitOffset(offset, limit);

    auto records = std::make_unique<std::vector<SMSTemplatesRecord>>(templates.begin(), templates.end());

    // for (const auto &w : smses) {

    //     auto contactRec = contactInterface.GetByID(w.contactID);
    //     // TODO: or numberUser? or other number
    //     records->push_back({w});
    // }

    return records;
}

bool SMSTemplatesRecordInterface::Update(const SMSTemplatesRecord &rec)
{
    auto templ = smsDB->templates.GetByID(rec.ID);
    if (templ.ID == DB_ID_NONE) {
        return false;
    }

    return smsDB->templates.Update(
        SMSTemplatesTableRow{{.ID = rec.ID}, .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
}

bool SMSTemplatesRecordInterface::RemoveByID(uint32_t id)
{

    auto templ = smsDB->templates.GetByID(id);
    if (templ.ID == 0) {
        return false;
    }

    return smsDB->templates.RemoveByID(id);
}

bool SMSTemplatesRecordInterface::RemoveByField(SMSTemplatesRecordField field, const char *str)
{
    assert(0 && "need implementation");
    return false;
}

SMSTemplatesRecord SMSTemplatesRecordInterface::GetByID(uint32_t id)
{
    auto templ = smsDB->templates.GetByID(id);

    return SMSTemplatesRecord{templ};
}
