// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSTemplateRecord.hpp"
#include "queries/messages/templates/QuerySMSTemplateGetByID.hpp"
#include "queries/messages/templates/QuerySMSTemplateGet.hpp"
#include "queries/messages/templates/QuerySMSTemplateAdd.hpp"
#include "queries/messages/templates/QuerySMSTemplateGetCount.hpp"
#include "queries/messages/templates/QuerySMSTemplateRemove.hpp"
#include "queries/messages/templates/QuerySMSTemplateUpdate.hpp"
#include <queries/messages/templates/QuerySMSTemplateGetForList.hpp>

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
        SMSTemplateTableRow{Record(rec.ID), .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
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
        SMSTemplateTableRow{Record(rec.ID), .text = rec.text, .lastUsageTimestamp = rec.lastUsageTimestamp});
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
        return getByIDQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateGet)) {
        return getQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateGetWithTotalCount)) {
        return getQueryWithTotalCount(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateGetForList)) {
        return getForListQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateGetCount)) {
        return getCountQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateAdd)) {
        return addQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateRemove)) {
        return removeQuery(query);
    }
    else if (typeid(*query) == typeid(db::query::SMSTemplateUpdate)) {
        return updateQuery(query);
    }
    else {
        return getByIDQuery(query);
    }
}

std::unique_ptr<db::QueryResult> SMSTemplateRecordInterface::getByIDQuery(const std::shared_ptr<db::Query> &query)
{
    const auto local_query = static_cast<const db::query::SMSTemplateGetByID *>(query.get());
    auto smsTemplate       = smsDB->templates.getById(local_query->id);

    auto response = std::make_unique<db::query::SMSTemplateGetByIDResult>(std::move(smsTemplate));
    response->setRequestQuery(query);
    return response;
}

auto SMSTemplateRecordInterface::getQueryRecords(const std::shared_ptr<db::Query> &query)
    -> std::vector<SMSTemplateRecord>
{
    const auto localQuery = static_cast<const db::query::SMSTemplateGet *>(query.get());
    auto dbResult         = smsDB->templates.getLimitOffset(localQuery->offset, localQuery->limit);

    return std::vector<SMSTemplateRecord>(dbResult.begin(), dbResult.end());
}

auto SMSTemplateRecordInterface::getQuery(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::SMSTemplateGetResult>(getQueryRecords(query));
    response->setRequestQuery(query);
    return response;
}

auto SMSTemplateRecordInterface::getQueryWithTotalCount(const std::shared_ptr<db::Query> &query)
    -> std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::SMSTemplateGetResultWithTotalCount>(getQueryRecords(query),
                                                                                    smsDB->templates.count());
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> SMSTemplateRecordInterface::getForListQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSTemplateGetForList *>(query.get());
    auto dbResult         = smsDB->templates.getLimitOffset(localQuery->offset, localQuery->limit);

    auto recordVector = std::vector<SMSTemplateRecord>(dbResult.begin(), dbResult.end());
    auto count        = smsDB->templates.count();

    auto response = std::make_unique<db::query::SMSTemplateGetForListResult>(std::move(recordVector), count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> SMSTemplateRecordInterface::getCountQuery(const std::shared_ptr<db::Query> &query)
{
    auto response = std::make_unique<db::query::SMSTemplateGetCountResult>(smsDB->templates.count());
    response->setRequestQuery(query);
    return response;
}
std::unique_ptr<db::QueryResult> SMSTemplateRecordInterface::addQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSTemplateAdd *>(query.get());
    auto ret              = SMSTemplateRecordInterface::Add(localQuery->rec);
    auto response         = std::make_unique<db::query::SMSTemplateAddResult>(ret);
    response->setRequestQuery(query);
    return response;
}
std::unique_ptr<db::QueryResult> SMSTemplateRecordInterface::removeQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSTemplateRemove *>(query.get());
    auto ret              = smsDB->templates.removeById(localQuery->id);
    auto response         = std::make_unique<db::query::SMSTemplateRemoveResult>(ret);
    response->setRequestQuery(query);
    return response;
}
std::unique_ptr<db::QueryResult> SMSTemplateRecordInterface::updateQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSTemplateUpdate *>(query.get());
    auto ret              = SMSTemplateRecordInterface::Update(localQuery->rec);
    auto response         = std::make_unique<db::query::SMSTemplateUpdateResult>(ret);
    response->setRequestQuery(query);
    return response;
}
