// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
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
        explicit QuotesAgent(Database *quotesDB, std::unique_ptr<settings::Settings> settings);
        ~QuotesAgent() = default;

        auto runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;

      protected:
        auto handleReadRandomizedQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;

      private:
        Database *quotesDB;
        ShuffleQuoteModel shuffleQuoteModel;
    };
} // namespace Quotes
