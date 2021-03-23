// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/QuotesMessages.hpp>
#include <Interface/Record.hpp>

namespace Quotes
{
    enum class QuotesRecordField
    {
    };

    class QuotesAgent : public RecordInterface<QuoteRecord, QuotesRecordField>
    {
      public:
        explicit QuotesAgent(Database *quotesDB);
        ~QuotesAgent() = default;

        auto runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>;

      protected:
        auto handleCategoryList(std::shared_ptr<db::Query> msg) -> std::unique_ptr<db::QueryResult>;
        // auto handleQuotesList(sys::Message *req) -> sys::MessagePointer;
        // auto handleQuotesListByCategoryId(sys::Message *req) -> sys::MessagePointer;
        // auto handleEnableCategoryById(sys::Message *req) -> sys::MessagePointer;
        // auto handleEnableQuoteById(sys::Message *req) -> sys::MessagePointer;
        // auto handleEnabledQuotesList(sys::Message *req) -> sys::MessagePointer;
        // auto handleAddQuote(sys::Message *req) -> sys::MessagePointer;
        // auto handleReadQuote(sys::Message *req) -> sys::MessagePointer;
        // auto handleWriteQuote(sys::Message *req) -> sys::MessagePointer;
        // auto handleDeleteQuote(sys::Message *req) -> sys::MessagePointer;

      private:
        Database *database;
    };
} // namespace Quotes
