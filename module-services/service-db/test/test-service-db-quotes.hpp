// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/QuotesMessages.hpp>
#include <service-db/agents/quotes/QuotesAgent.hpp>

namespace Quotes
{
    class QuotesAgentTester : public QuotesAgent
    {
      public:
        QuotesAgentTester(Database *quotesDB) : QuotesAgent(quotesDB){};
        ~QuotesAgentTester() = default;

        auto handleCategoryList(std::shared_ptr<Messages::GetCategoryListRequest> query)
            -> std::unique_ptr<db::QueryResult>
        {
            return QuotesAgent::handleCategoryList(query);
        }

        auto getAllQuotes(unsigned int limit = 0, unsigned int offset = 0) -> std::vector<QuoteRecord>
        {
            auto request     = std::make_shared<Messages::GetQuotesListRequest>(offset, limit);
            auto queryResult = handleQuotesList(request);
            auto response    = dynamic_cast<Messages::GetQuotesListResponse *>(queryResult.get());
            return response->getResults();
        }

        auto getQuotesFromCustomCategory() -> std::vector<QuoteRecord>
        {
            unsigned int offset = 0;
            unsigned int limit  = 0;

            auto request     = std::make_shared<Messages::GetQuotesListFromCustomCategoryRequest>(offset, limit);
            auto queryResult = handleQuotesListFromCustomCategory(request);
            auto response    = dynamic_cast<Messages::GetQuotesListFromCustomCategoryResponse *>(queryResult.get());
            return response->getResults();
        }

        auto getQuotesByCategoryId(unsigned int categoryId) -> std::vector<QuoteRecord>
        {
            unsigned int offset = 0;
            unsigned int limit  = 0;

            auto request     = std::make_shared<Messages::GetQuotesListByCategoryIdRequest>(offset, limit, categoryId);
            auto queryResult = handleQuotesListByCategoryId(request);
            auto response    = dynamic_cast<Messages::GetQuotesListByCategoryIdResponse *>(queryResult.get());
            return response->getResults();
        }

        auto getEnabledQuotes() -> std::vector<QuoteRecord>
        {
            unsigned int offset = 0;
            unsigned int limit  = 0;

            auto request     = std::make_shared<Messages::GetEnabledQuotesListRequest>(offset, limit);
            auto queryResult = handleEnabledQuotesList(request);
            auto response    = dynamic_cast<Messages::GetEnabledQuotesListResponse *>(queryResult.get());
            return response->getResults();
        }

        auto enableCategory(unsigned int categoryId, bool enable) -> bool
        {
            auto request     = std::make_shared<Messages::EnableCategoryByIdRequest>(categoryId, enable);
            auto queryResult = handleEnableCategoryById(request);
            auto response    = dynamic_cast<Messages::EnableCategoryByIdResponse *>(queryResult.get());
            return response->success;
        }

        auto enableQuote(unsigned int quoteId, bool enable) -> bool
        {
            auto request     = std::make_shared<Messages::EnableQuoteByIdRequest>(quoteId, enable);
            auto queryResult = handleEnableQuoteById(request);
            auto response    = dynamic_cast<Messages::EnableQuoteByIdResponse *>(queryResult.get());
            return response->success;
        }

        auto addQuote(unsigned int langId, std::string quote, std::string author, bool enabled) -> unsigned int
        {
            auto request =
                std::make_shared<Messages::AddQuoteRequest>(langId, std::move(quote), std::move(author), enabled);
            auto queryResult = handleAddQuote(request);
            auto response    = dynamic_cast<Messages::AddQuoteResponse *>(queryResult.get());
            return response->quoteId;
        }

        auto readQuote(unsigned int quoteId) -> std::unique_ptr<db::QueryResult>
        {
            auto request = std::make_shared<Messages::ReadQuoteRequest>(quoteId);
            return handleReadQuote(request);
        }

        auto writeQuote(unsigned int quoteId, unsigned int langId, std::string quote, std::string author, bool enabled)
            -> bool
        {
            auto request = std::make_shared<Messages::WriteQuoteRequest>(
                quoteId, langId, std::move(quote), std::move(author), enabled);
            auto queryResult = handleWriteQuote(request);
            auto response    = dynamic_cast<Messages::WriteQuoteResponse *>(queryResult.get());
            return response->success;
        }

        auto deleteQuote(unsigned int quoteId) -> bool
        {
            auto request     = std::make_shared<Messages::DeleteQuoteRequest>(quoteId);
            auto queryResult = handleDeleteQuote(request);
            auto response    = dynamic_cast<Messages::DeleteQuoteResponse *>(queryResult.get());
            return response->success;
        }
    };
} // namespace Quotes
