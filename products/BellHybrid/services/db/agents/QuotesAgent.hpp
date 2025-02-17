// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ShuffleQuoteModel.hpp"

#include <Interface/Record.hpp>
#include <service-db/Settings.hpp>

namespace Quotes
{
    enum class QuotesRecordField
    {
    };

    class QuotesAgent : public RecordInterface<QuoteRecord, QuotesRecordField>
    {
      public:
        QuotesAgent(Database *quotesDB, std::unique_ptr<settings::Settings> settings);
        ~QuotesAgent() = default;

        auto runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;

      protected:
        auto handleReadRandomizedQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;

      private:
        std::unique_ptr<settings::Settings> settings;
        Database *quotesDB;
        ShuffleQuoteModel shuffleQuoteModel;

        auto handleAddNewEntry(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleEditEntry(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleDeleteEntry(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
        auto handleGroupChanged(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;
    };
} // namespace Quotes
