#include "SMSTemplateRecord.hpp"
#include "queries/sms/QuerySMSTemplateGetByID.hpp"
#include "queries/sms/QuerySMSTemplateGet.hpp"
#include "queries/sms/QuerySMSTemplateAdd.hpp"
#include "queries/sms/QuerySMSTemplateGetCount.hpp"
#include "queries/sms/QuerySMSTemplateRemove.hpp"
#include "queries/sms/QuerySMSThreadsGet.hpp"

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

bool SMSTemplateRecordInterface::Add(const SMSTemplateRecord &rec)
{
    return smsDB->templates.add(
        SMSTemplateTableRow{{.ID = rec.ID}, .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
}
uint32_t SMSTemplateRecordInterface::GetCount()
{
    return smsDB->templates.count();
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
    auto templates = smsDB->templates.getLimitOffset(offset, limit);

    return std::make_unique<std::vector<SMSTemplateRecord>>(templates.begin(), templates.end());
}

bool SMSTemplateRecordInterface::Update(const SMSTemplateRecord &rec)
{
    auto templ = smsDB->templates.getById(rec.ID);
    if (templ.ID == DB_ID_NONE) {
        return false;
    }

    return smsDB->templates.update(
        SMSTemplateTableRow{{.ID = rec.ID}, .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
}

bool SMSTemplateRecordInterface::RemoveByID(uint32_t id)
{

    auto templ = smsDB->templates.getById(id);
    if (templ.ID == 0) {
        return false;
    }

    return smsDB->templates.removeById(id);
}

bool SMSTemplateRecordInterface::RemoveByField(SMSTemplateRecordField field, const char *str)
{
    assert(0 && "need implementation");
    return false;
}

SMSTemplateRecord SMSTemplateRecordInterface::GetByID(uint32_t id)
{
    auto templ = smsDB->templates.getById(id);

    return SMSTemplateRecord{templ};
}

std::unique_ptr<db::QueryResult> SMSTemplateRecordInterface::runQuery(std::shared_ptr<db::Query> query)
{
    if (typeid(*query) == typeid(db::query::SMSTemplateGetByID)) {
        const auto local_query = dynamic_cast<const db::query::SMSTemplateGetByID *>(query.get());
        auto smsTemplate       = smsDB->templates.getById(local_query->id);

        auto response = std::make_unique<db::query::SMSTemplateGetByIDResult>(std::move(smsTemplate));
        response->setRequestQuery(query);
        return response;
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateGet)) {
        const auto localQuery = dynamic_cast<const db::query::SMSTemplateGet *>(query.get());
        auto dbResult         = smsDB->templates.getLimitOffset(localQuery->offset, localQuery->limit);
        std::vector<SMSTemplateRecord> recordVector;
        for (auto SMStemplate : dbResult) {
            SMSTemplateRecord record;
            record.ID                 = SMStemplate.ID;
            record.text               = SMStemplate.text;
            record.lastUsageTimestamp = SMStemplate.lastUsageTimestamp;
            recordVector.emplace_back(record);
        }
        auto response = std::make_unique<db::query::SMSTemplateGetResult>(std::move(recordVector));
        response->setRequestQuery(query);
        return response;
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateGetCount)) {
        auto response = std::make_unique<db::query::SMSTemplateGetCountResult>(smsDB->templates.count());
        response->setRequestQuery(query);
        return response;
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateAdd)) {
        const auto localQuery = dynamic_cast<const db::query::SMSTemplateAdd *>(query.get());
        auto ret              = SMSTemplateRecordInterface::Add(localQuery->rec);
        auto response         = std::make_unique<db::query::SMSTemplateAddResult>(ret);
        response->setRequestQuery(query);
        return response;
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateRemove)) {
        const auto localQuery = dynamic_cast<const db::query::SMSTemplateRemove *>(query.get());
        auto ret              = smsDB->templates.removeById(localQuery->id);
        auto response         = std::make_unique<db::query::SMSTemplateRemoveResult>(ret);
        response->setRequestQuery(query);
        return response;
    }
    else {
        auto response = std::make_unique<db::query::SMSTemplateGetByIDResult>(SMSTemplateRecord());
        response->setRequestQuery(query);
        return response;
    }
}
