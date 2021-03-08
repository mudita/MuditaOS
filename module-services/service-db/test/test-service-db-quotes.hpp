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
    };
} // namespace Quotes
