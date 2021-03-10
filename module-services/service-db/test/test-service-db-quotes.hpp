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
        QuotesAgentTester(sys::Service *parentService) : QuotesAgent(parentService){};
        ~QuotesAgentTester() = default;

        auto handleCategoryList(Messages::GetCategoryListRequest *req) -> sys::MessagePointer
        {
            return QuotesAgent::handleCategoryList(req);
        }

        auto handleQuotesList(Messages::GetQuotesListRequest *req) -> sys::MessagePointer
        {
            return QuotesAgent::handleQuotesList(req);
        }

        auto handleQuotesListByCategoryId(Messages::GetQuotesListByCategoryIdRequest *req) -> sys::MessagePointer
        {
            return QuotesAgent::handleQuotesListByCategoryId(req);
        }

        auto handleEnableCategoryById(Messages::EnableCategoryByIdRequest *req) -> sys::MessagePointer
        {
            return QuotesAgent::handleEnableCategoryById(req);
        }

        auto handleEnableQuoteById(Messages::EnableQuoteByIdRequest *req) -> sys::MessagePointer
        {
            return QuotesAgent::handleEnableQuoteById(req);
        }

        auto handleEnabledQuotesList(Messages::GetEnabledQuotesListRequest *req) -> sys::MessagePointer
        {
            return QuotesAgent::handleEnabledQuotesList(req);
        }

        auto getQuotesByCategoryId(unsigned int categoryId) -> std::vector<QuoteRecord>
        {
            QuotesList quotesList;
            quotesList.limit = 0;

            auto record   = std::make_unique<QuotesList>(quotesList);
            auto request  = std::make_shared<Messages::GetQuotesListByCategoryIdRequest>(std::move(record), categoryId);
            auto response = std::dynamic_pointer_cast<Messages::GetQuotesListByCategoryIdResponse>(
                handleQuotesListByCategoryId(request.get()));

            return response->getResults();
        }

        auto getEnabledQuotes() -> std::vector<QuoteRecord>
        {
            QuotesList quotesList;
            quotesList.limit = 0;

            auto record   = std::make_unique<QuotesList>(quotesList);
            auto request  = std::make_shared<Messages::GetEnabledQuotesListRequest>(std::move(record));
            auto response = std::dynamic_pointer_cast<Messages::GetEnabledQuotesListResponse>(
                handleEnabledQuotesList(request.get()));

            return response->getResults();
        }

        auto enableCategory(unsigned int categoryId, bool enable)
            -> std::shared_ptr<Messages::EnableCategoryByIdResponse>
        {
            auto request = std::make_shared<Messages::EnableCategoryByIdRequest>(categoryId, enable);
            return std::dynamic_pointer_cast<Messages::EnableCategoryByIdResponse>(
                handleEnableCategoryById(request.get()));
        }

        auto enableQuote(unsigned int quoteId, bool enable) -> std::shared_ptr<Messages::EnableQuoteByIdResponse>
        {
            auto request = std::make_shared<Messages::EnableQuoteByIdRequest>(quoteId, enable);
            return std::dynamic_pointer_cast<Messages::EnableQuoteByIdResponse>(handleEnableQuoteById(request.get()));
        }
    };
} // namespace Quotes
