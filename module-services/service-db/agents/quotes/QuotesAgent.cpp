// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesAgent.hpp"
#include "QuotesQueries.hpp"

//#include <Application.hpp>
#include <Common/Query.hpp>

namespace Quotes
{
    QuotesAgent::QuotesAgent(Database *quotesDB) : database(quotesDB)
    {
    }

    auto QuotesAgent::runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        if (typeid(*query) == typeid(Messages::GetCategoryListRequest)) {
            return handleCategoryList(query);
        }
        else if (typeid(*query) == typeid(Messages::EnableCategoryByIdRequest)) {
            return handleEnableCategoryById(query);
        }
        else if (typeid(*query) == typeid(Messages::GetQuotesListRequest)) {
            return handleQuotesList(query);
        }
        else if (typeid(*query) == typeid(Messages::GetQuotesListByCategoryIdRequest)) {
            return handleQuotesListByCategoryId(query);
        }
        else if (typeid(*query) == typeid(Messages::GetQuotesListFromCustomCategoryRequest)) {
            return handleQuotesListFromCustomCategory(query);
        }
        else if (typeid(*query) == typeid(Messages::GetEnabledQuotesListRequest)) {
            return handleEnabledQuotesList(query);
        }
        else if (typeid(*query) == typeid(Messages::EnableQuoteByIdRequest)) {
            return handleEnableQuoteById(query);
        }
        else if (typeid(*query) == typeid(Messages::AddQuoteRequest)) {
            return handleAddQuote(query);
        }
        else if (typeid(*query) == typeid(Messages::ReadQuoteRequest)) {
            return handleReadQuote(query);
        }
        else if (typeid(*query) == typeid(Messages::WriteQuoteRequest)) {
            return handleWriteQuote(query);
        }
        else if (typeid(*query) == typeid(Messages::DeleteQuoteRequest)) {
            return handleDeleteQuote(query);
        }
        return nullptr;
    }

    auto QuotesAgent::handleCategoryList(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req          = std::dynamic_pointer_cast<Messages::GetCategoryListRequest>(query);
        auto queryResult  = database->query(Queries::getAllCategories);
        auto categoryList = getList<CategoryList, CategoryRecord>(req->offset, req->limit, std::move(queryResult));
        auto response     = std::make_unique<Messages::GetCategoryListResponse>(std::move(categoryList));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleEnableCategoryById(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::EnableCategoryByIdRequest>(query);
        auto queryResult = database->execute(Queries::enableCategory, req->enable, req->categoryId);
        auto response    = std::make_unique<Messages::EnableCategoryByIdResponse>(std::move(queryResult));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleQuotesList(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::GetQuotesListRequest>(query);
        auto queryResult = database->query(Queries::getAllQuotes);
        auto quotesList  = getList<QuotesList, QuoteRecord>(req->offset, req->limit, std::move(queryResult));
        auto response    = std::make_unique<Messages::GetQuotesListResponse>(std::move(quotesList));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleQuotesListByCategoryId(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::GetQuotesListByCategoryIdRequest>(query);
        auto queryResult = database->query(Queries::getQuotesByCategoryId, req->categoryId);
        auto quotesList  = getList<QuotesList, QuoteRecord>(req->offset, req->limit, std::move(queryResult));
        auto response    = std::make_unique<Messages::GetQuotesListByCategoryIdResponse>(std::move(quotesList));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleQuotesListFromCustomCategory(std::shared_ptr<db::Query> query)
        -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::GetQuotesListFromCustomCategoryRequest>(query);
        auto queryResult = database->query(Queries::getQuotesFromCustomCategory);
        auto quotesList  = getList<QuotesList, QuoteRecord>(req->offset, req->limit, std::move(queryResult));
        auto response    = std::make_unique<Messages::GetQuotesListFromCustomCategoryResponse>(std::move(quotesList));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleEnabledQuotesList(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::GetEnabledQuotesListRequest>(query);
        auto queryResult = database->query(Queries::getEnabledQuotes);
        auto quotesList  = getList<QuotesList, QuoteRecord>(req->offset, req->limit, std::move(queryResult));
        auto response    = std::make_unique<Messages::GetEnabledQuotesListResponse>(std::move(quotesList));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleEnableQuoteById(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::EnableQuoteByIdRequest>(query);
        auto queryResult = database->execute(Queries::enableQuote, req->enable, req->quoteId);
        auto response    = std::make_unique<Messages::EnableQuoteByIdResponse>(queryResult);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleAddQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req = std::dynamic_pointer_cast<Messages::AddQuoteRequest>(query);

        database->execute(
            Queries::addQuoteToQuoteTable, req->langId, req->quote.c_str(), req->author.c_str(), req->enabled);

        auto quoteId = database->getLastInsertRowId();

        auto queryResult = database->query(Queries::getCustomCategoryId);
        CategoryRecord categoryRecord(queryResult.get());

        auto success = database->execute(Queries::addQuoteToQuoteCategoryMapTable, categoryRecord.category_id, quoteId);

        auto response = std::make_unique<Messages::AddQuoteResponse>(success, quoteId);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleReadQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::ReadQuoteRequest>(query);
        auto queryResult = database->query(Queries::readQuote, req->quoteId);
        QuoteRecord quoteRecord(queryResult.get());
        auto response = std::make_unique<Messages::ReadQuoteResponse>(
            quoteRecord.quote_id, quoteRecord.lang_id, quoteRecord.quote, quoteRecord.author, quoteRecord.enabled);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleWriteQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::WriteQuoteRequest>(query);
        auto queryResult = database->execute(
            Queries::writeQuote, req->langId, req->quote.c_str(), req->author.c_str(), req->enabled, req->quoteId);
        auto response = std::make_unique<Messages::WriteQuoteResponse>(queryResult);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleDeleteQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req = std::dynamic_pointer_cast<Messages::DeleteQuoteRequest>(query);
        database->execute(Queries::deleteQuoteFromQuoteCategoryMapTable, req->quoteId);

        auto queryResult = database->execute(Queries::deleteQuoteFromQuoteTable, req->quoteId);

        auto response = std::make_unique<Messages::DeleteQuoteResponse>(queryResult);
        response->setRequestQuery(query);
        return response;
    }
} // namespace Quotes
