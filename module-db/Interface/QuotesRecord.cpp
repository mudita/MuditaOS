// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuoteRecord.hpp"
#include <service-db/QuotesMessages.hpp>
#include <log/log.hpp>

#include <cassert>

QuotesRecordInterface::QuotesRecordInterface(QuotesDB *quotesDB) : quotesDB(quotesDB)
{}

uint32_t QuotesRecordInterface::GetCount()
{
    return quotesDB->quotes.count();
}

std::unique_ptr<std::vector<Quotes::QuoteRecord>> QuotesRecordInterface::GetLimitOffsetByField(uint32_t offset,
                                                                                               uint32_t limit,
                                                                                               QuotesRecordField field,
                                                                                               const char *str)
{
    assert(0 && "need proper implementation");
    return GetLimitOffset(offset, limit);
}

std::unique_ptr<std::vector<Quotes::QuoteRecord>> QuotesRecordInterface::GetLimitOffset(uint32_t offset, uint32_t limit)
{
    auto quotes = quotesDB->quotes.getLimitOffset(offset, limit);

    return std::make_unique<std::vector<Quotes::QuoteRecord>>(quotes.begin(), quotes.end());
}

std::unique_ptr<db::QueryResult> QuotesRecordInterface::runQuery(std::shared_ptr<db::Query> query)
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
    else {
        return getByIDQuery(query);
    }
}

auto QuotesRecordInterface::getQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<Quotes::QuoteRecord>
{
    const auto localQuery = static_cast<const db::query::SMSTemplateGet *>(query.get());
    auto dbResult         = quotesDB->quotes.getLimitOffset(localQuery->offset, localQuery->limit);

    return std::vector<Quotes::QuoteRecord>(dbResult.begin(), dbResult.end());
}

auto QuotesRecordInterface::getQuery(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::SMSTemplateGetResult>(getQueryRecords(query));
    response->setRequestQuery(query);
    return response;
}

auto QuotesRecordInterface::getQueryWithTotalCount(const std::shared_ptr<db::Query> &query)
    -> std::unique_ptr<db::QueryResult>
{
    auto response = std::make_unique<db::query::SMSTemplateGetResultWithTotalCount>(getQueryRecords(query),
                                                                                    quotesDB->quotes.count());
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> QuotesRecordInterface::getForListQuery(const std::shared_ptr<db::Query> &query)
{
    const auto localQuery = static_cast<const db::query::SMSTemplateGetForList *>(query.get());
    auto dbResult         = quotesDB->quotes.getLimitOffset(localQuery->offset, localQuery->limit);

    auto recordVector = std::vector<Quotes::QuoteRecord>(dbResult.begin(), dbResult.end());
    auto count        = quotesDB->quotes.count();

    auto response = std::make_unique<db::query::SMSTemplateGetForListResult>(std::move(recordVector), count);
    response->setRequestQuery(query);
    return response;
}

std::unique_ptr<db::QueryResult> QuotesRecordInterface::getCountQuery(const std::shared_ptr<db::Query> &query)
{
    auto response = std::make_unique<db::query::SMSTemplateGetCountResult>(quotesDB->quotes.count());
    response->setRequestQuery(query);
    return response;
}
