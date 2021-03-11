// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/DatabaseAgent.hpp>
#include <service-db/QuotesMessages.hpp>

namespace Quotes
{
    class QuotesAgent : public DatabaseAgent
    {
      public:
        explicit QuotesAgent(sys::Service *parentService);
        ~QuotesAgent() = default;

        void initDb() override
        {}
        void deinitDb() override
        {}
        void registerMessages() override;
        auto getAgentName() -> const std::string override;

      protected:
        auto handleCategoryList(sys::Message *req) -> sys::MessagePointer;
        auto handleQuotesList(sys::Message *req) -> sys::MessagePointer;
        auto handleQuotesListByCategoryId(sys::Message *req) -> sys::MessagePointer;
        auto handleEnableCategoryById(sys::Message *req) -> sys::MessagePointer;
        auto handleEnableQuoteById(sys::Message *req) -> sys::MessagePointer;
        auto handleEnabledQuotesList(sys::Message *req) -> sys::MessagePointer;
        auto handleAddQuote(sys::Message *req) -> sys::MessagePointer;
        auto handleReadQuote(sys::Message *req) -> sys::MessagePointer;
        auto handleWriteQuote(sys::Message *req) -> sys::MessagePointer;
        auto handleDeleteQuote(sys::Message *req) -> sys::MessagePointer;

      private:
        auto getDbInitString() -> const std::string override;
        auto getDbFilePath() -> const std::string override;
    };
} // namespace Quotes
