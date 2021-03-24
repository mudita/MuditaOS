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

        // auto handleQuotesList(Messages::GetQuotesListRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleQuotesList(req);
        // }

        // auto handleQuotesListByCategoryId(Messages::GetQuotesListByCategoryIdRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleQuotesListByCategoryId(req);
        // }

        // auto handleEnableCategoryById(Messages::EnableCategoryByIdRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleEnableCategoryById(req);
        // }

        // auto handleEnableQuoteById(Messages::EnableQuoteByIdRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleEnableQuoteById(req);
        // }

        // auto handleEnabledQuotesList(Messages::GetEnabledQuotesListRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleEnabledQuotesList(req);
        // }

        // auto handleAddQuote(Messages::AddQuoteRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleAddQuote(req);
        // }

        // auto handleReadQuote(Messages::ReadQuoteRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleReadQuote(req);
        // }

        // auto handleWriteQuote(Messages::WriteQuoteRequest *req) -> sys::MessagePointer
        // {
        //     return QuotesAgent::handleWriteQuote(req);
        // }

        // auto getAllQuotes(unsigned int limit = 0, unsigned int offset = 0)
        //     -> std::shared_ptr<Messages::GetQuotesListResponse>
        // {
        //     QuotesList quotesList;
        //     quotesList.limit  = limit;
        //     quotesList.offset = offset;

        //     auto record  = std::make_unique<QuotesList>(quotesList);
        //     auto request = std::make_shared<Messages::GetQuotesListRequest>(std::move(record));
        //     return std::dynamic_pointer_cast<Messages::GetQuotesListResponse>(handleQuotesList(request.get()));
        // }

        // auto getQuotesByCategoryId(unsigned int categoryId) -> std::vector<QuoteRecord>
        // {
        //     QuotesList quotesList;
        //     quotesList.limit = 0;

        //     auto record   = std::make_unique<QuotesList>(quotesList);
        //     auto request  = std::make_shared<Messages::GetQuotesListByCategoryIdRequest>(std::move(record),
        //     categoryId); auto response = std::dynamic_pointer_cast<Messages::GetQuotesListByCategoryIdResponse>(
        //         handleQuotesListByCategoryId(request.get()));

        //     return response->getResults();
        // }

        // auto getEnabledQuotes() -> std::vector<QuoteRecord>
        // {
        //     QuotesList quotesList;
        //     quotesList.limit = 0;

        //     auto record   = std::make_unique<QuotesList>(quotesList);
        //     auto request  = std::make_shared<Messages::GetEnabledQuotesListRequest>(std::move(record));
        //     auto response = std::dynamic_pointer_cast<Messages::GetEnabledQuotesListResponse>(
        //         handleEnabledQuotesList(request.get()));

        //     return response->getResults();
        // }

        // auto enableCategory(unsigned int categoryId, bool enable)
        //     -> std::shared_ptr<Messages::EnableCategoryByIdResponse>
        // {
        //     auto request = std::make_shared<Messages::EnableCategoryByIdRequest>(categoryId, enable);
        //     return std::dynamic_pointer_cast<Messages::EnableCategoryByIdResponse>(
        //         handleEnableCategoryById(request.get()));
        // }

        // auto enableQuote(unsigned int quoteId, bool enable) -> std::shared_ptr<Messages::EnableQuoteByIdResponse>
        // {
        //     auto request = std::make_shared<Messages::EnableQuoteByIdRequest>(quoteId, enable);
        //     return
        //     std::dynamic_pointer_cast<Messages::EnableQuoteByIdResponse>(handleEnableQuoteById(request.get()));
        // }

        // auto addQuote(unsigned int langId, std::string quote, std::string author, bool enabled)
        //     -> std::shared_ptr<Messages::AddQuoteResponse>
        // {
        //     auto request =
        //         std::make_shared<Messages::AddQuoteRequest>(langId, std::move(quote), std::move(author), enabled);
        //     return std::dynamic_pointer_cast<Messages::AddQuoteResponse>(handleAddQuote(request.get()));
        // }

        // auto readQuote(unsigned int quoteId) -> std::shared_ptr<Messages::ReadQuoteResponse>
        // {
        //     auto request = std::make_shared<Messages::ReadQuoteRequest>(quoteId);
        //     return std::dynamic_pointer_cast<Messages::ReadQuoteResponse>(handleReadQuote(request.get()));
        // }

        // auto writeQuote(unsigned int quoteId, unsigned int langId, std::string quote, std::string author, bool
        // enabled)
        //     -> std::shared_ptr<Messages::WriteQuoteResponse>
        // {
        //     auto request = std::make_shared<Messages::WriteQuoteRequest>(
        //         quoteId, langId, std::move(quote), std::move(author), enabled);
        //     return std::dynamic_pointer_cast<Messages::WriteQuoteResponse>(handleWriteQuote(request.get()));
        // }

        // auto deleteQuote(unsigned int quoteId) -> std::shared_ptr<Messages::DeleteQuoteResponse>
        // {
        //     auto request = std::make_shared<Messages::DeleteQuoteRequest>(quoteId);
        //     return std::dynamic_pointer_cast<Messages::DeleteQuoteResponse>(handleDeleteQuote(request.get()));
        // }
    };
} // namespace Quotes
