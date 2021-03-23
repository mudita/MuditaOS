// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include "Databases/QuotesDB.hpp"
#include "Common/Common.hpp"
#include <service-db/service-db/QuotesMessages.hpp>
#include <utf8/UTF8.hpp>
#include <stdint.h>

enum class QuotesRecordField
{
};

class QuotesRecordInterface : public RecordInterface<Quotes::QuoteRecord, QuotesRecordField>
{
  public:
    ~QuotesRecordInterface() = default;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<Quotes::QuoteRecord>> GetLimitOffset(uint32_t offset, uint32_t limit) override final;

    std::unique_ptr<std::vector<Quotes::QuoteRecord>> GetLimitOffsetByField(uint32_t offset,
                                                                            uint32_t limit,
                                                                            QuotesRecordField field,
                                                                            const char *str) override final;

    std::unique_ptr<db::QueryResult> runQuery(std::shared_ptr<db::Query> query) override;

  private:
    QuotesDB *quotesDB;

    std::unique_ptr<db::QueryResult> getByIDQuery(const std::shared_ptr<db::Query> &query);
    auto getQueryRecords(const std::shared_ptr<db::Query> &query) -> std::vector<Quotes::QuoteRecord>;
    auto getQuery(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>;
    auto getQueryWithTotalCount(const std::shared_ptr<db::Query> &query) -> std::unique_ptr<db::QueryResult>;
    std::unique_ptr<db::QueryResult> getForListQuery(const std::shared_ptr<db::Query> &query);
    std::unique_ptr<db::QueryResult> getCountQuery(const std::shared_ptr<db::Query> &query);
};
