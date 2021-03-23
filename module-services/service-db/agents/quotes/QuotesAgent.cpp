// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesAgent.hpp"
#include "QuotesQueries.hpp"

#include <Application.hpp>
#include <Common/Query.hpp>
#include <purefs/filesystem_paths.hpp>

namespace Quotes
{
    QuotesAgent::QuotesAgent(Database *quotesDB) : database(quotesDB)
    {
    }

    auto QuotesAgent::runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        LOG_ERROR("runQuery");
        if (typeid(*query) == typeid(Messages::GetCategoryListRequest)) {
            LOG_ERROR("GetCategoryListRequest");
            return handleCategoryList(query);
        }
        return nullptr;
    }

    //     void QuotesAgent::registerMessages()
    //     {
    //         using std::placeholders::_1;
    //
    //         parentService->connect(typeid(Messages::GetCategoryListRequest),
    //                                std::bind(&QuotesAgent::handleCategoryList, this, _1));
    //         parentService->connect(typeid(Messages::GetQuotesListRequest),
    //                                std::bind(&QuotesAgent::handleQuotesList, this, _1));
    //         parentService->connect(typeid(Messages::GetQuotesListByCategoryIdRequest),
    //                                std::bind(&QuotesAgent::handleQuotesListByCategoryId, this, _1));
    //         parentService->connect(typeid(Messages::EnableCategoryByIdRequest),
    //                                std::bind(&QuotesAgent::handleEnableCategoryById, this, _1));
    //         parentService->connect(typeid(Messages::EnableQuoteByIdRequest),
    //                                std::bind(&QuotesAgent::handleEnableQuoteById, this, _1));
    //         parentService->connect(typeid(Messages::GetEnabledQuotesListRequest),
    //                                std::bind(&QuotesAgent::handleEnabledQuotesList, this, _1));
    //         parentService->connect(typeid(Messages::AddQuoteRequest), std::bind(&QuotesAgent::handleAddQuote, this,
    //         _1)); parentService->connect(typeid(Messages::ReadQuoteRequest), std::bind(&QuotesAgent::handleReadQuote,
    //         this, _1)); parentService->connect(typeid(Messages::WriteQuoteRequest),
    //                                std::bind(&QuotesAgent::handleWriteQuote, this, _1));
    //         parentService->connect(typeid(Messages::DeleteQuoteRequest),
    //                                std::bind(&QuotesAgent::handleDeleteQuote, this, _1));
    //     }

    auto QuotesAgent::handleCategoryList(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req          = std::dynamic_pointer_cast<Messages::GetCategoryListRequest>(query);
        auto queryResult  = database->query(Queries::getAllCategories);
        auto categoryList = getList<CategoryList, CategoryRecord>(req->limit, req->offset, std::move(queryResult));
        LOG_ERROR("categoryList->count = %d", categoryList->count);
        auto response = std::make_unique<Messages::GetCategoryListResponse>(std::move(categoryList));
        response->setRequestQuery(query);
        return response;
    }

    // auto QuotesAgent::handleQuotesList(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::GetQuotesListRequest *>(req)) {
    //         auto query      = database->query(Queries::getAllQuotes);
    //         auto quotesList = getList<QuotesList, QuoteRecord>(msg->limit, msg->offset, std::move(query));
    //         return std::make_shared<Messages::GetQuotesListResponse>(std::move(quotesList));
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleQuotesListByCategoryId(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::GetQuotesListByCategoryIdRequest *>(req)) {
    //         auto query      = database->query(Queries::getQuotesByCategoryId, msg->categoryId);
    //         auto quotesList = getList<QuotesList, QuoteRecord>(msg->limit, msg->offset, std::move(query));
    //         return std::make_shared<Messages::GetQuotesListByCategoryIdResponse>(std::move(quotesList));
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleEnableCategoryById(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::EnableCategoryByIdRequest *>(req)) {
    //         auto result = database->execute(Queries::enableCategory, msg->enable, msg->categoryId);
    //         return std::make_shared<Messages::EnableCategoryByIdResponse>(result);
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleEnableQuoteById(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::EnableQuoteByIdRequest *>(req)) {
    //         auto result = database->execute(Queries::enableQuote, msg->enable, msg->quoteId);
    //         return std::make_shared<Messages::EnableQuoteByIdResponse>(result);
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleEnabledQuotesList(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::GetEnabledQuotesListRequest *>(req)) {
    //         auto query      = database->query(Queries::getEnabledQuotes);
    //         auto quotesList = getList<QuotesList, QuoteRecord>(msg->limit, msg->offset, std::move(query));
    //         return std::make_shared<Messages::GetEnabledQuotesListResponse>(std::move(quotesList));
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleAddQuote(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::AddQuoteRequest *>(req)) {
    //         auto result = database->execute(
    //             Queries::addQuote, msg->langId, msg->quote.c_str(), msg->author.c_str(), msg->enabled);
    //         auto quoteId = database->getLastInsertRowId();
    //         return std::make_shared<Messages::AddQuoteResponse>(result, quoteId);
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleReadQuote(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::ReadQuoteRequest *>(req)) {
    //         auto result = database->query(Queries::readQuote, msg->quoteId);
    //         QuoteRecord quoteRecord(result.get());
    //         return std::make_shared<Messages::ReadQuoteResponse>(
    //             quoteRecord.quote_id, quoteRecord.lang_id, quoteRecord.quote, quoteRecord.author,
    //             quoteRecord.enabled);
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleWriteQuote(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::WriteQuoteRequest *>(req)) {
    //         auto result = database->execute(
    //             Queries::writeQuote, msg->langId, msg->quote.c_str(), msg->author.c_str(), msg->enabled,
    //             msg->quoteId);
    //         return std::make_shared<Messages::WriteQuoteResponse>(result);
    //     }
    //     return sys::msgNotHandled();
    // }

    // auto QuotesAgent::handleDeleteQuote(sys::Message *req) -> sys::MessagePointer
    // {
    //     if (auto msg = dynamic_cast<Messages::DeleteQuoteRequest *>(req)) {
    //         auto result = database->execute(Queries::deleteQuote, msg->quoteId);
    //         return std::make_shared<Messages::DeleteQuoteResponse>(result);
    //     }
    //     return sys::msgNotHandled();
    // }

} // namespace Quotes
