// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesAgent.hpp"
#include "QuotesQueries.hpp"

#include <Application.hpp>
#include <purefs/filesystem_paths.hpp>

namespace Quotes
{
    QuotesAgent::QuotesAgent(sys::Service *parentService) : DatabaseAgent(parentService)
    {
        database = std::make_unique<Database>(getDbFilePath().c_str());
    }

    void QuotesAgent::registerMessages()
    {
        using std::placeholders::_1;

        parentService->connect(Messages::GetCategoryListRequest(),
                               std::bind(&QuotesAgent::handleCategoryList, this, _1));
        parentService->connect(Messages::GetQuotesListRequest(), std::bind(&QuotesAgent::handleQuotesList, this, _1));
    }

    auto QuotesAgent::getDbFilePath() -> const std::string
    {
        return (purefs::dir::getUserDiskPath() / "quotes.db").string();
    }

    auto QuotesAgent::getDbInitString() -> const std::string
    {
        return {};
    }

    auto QuotesAgent::getAgentName() -> const std::string
    {
        return std::string("quotesAgent");
    }

    auto QuotesAgent::handleCategoryList(sys::Message *req) -> sys::MessagePointer
    {
        if (auto msg = dynamic_cast<Messages::GetCategoryListRequest *>(req)) {
            auto query        = database->query(Queries::getAllCategories);
            auto categoryList = getList<CategoryList, CategoryRecord>(std::move(msg->categoryList), std::move(query));
            return std::make_shared<Messages::GetCategoryListResponse>(std::move(categoryList));
        }
        return app::msgNotHandled();
    }

    auto QuotesAgent::handleQuotesList(sys::Message *req) -> sys::MessagePointer
    {
        if (auto msg = dynamic_cast<Messages::GetQuotesListRequest *>(req)) {
            auto query      = database->query(Queries::getAllQuotes);
            auto quotesList = getList<QuotesList, QuoteRecord>(std::move(msg->quotesList), std::move(query));
            return std::make_shared<Messages::GetQuotesListResponse>(std::move(quotesList));
        }
        return app::msgNotHandled();
    }

} // namespace Quotes
