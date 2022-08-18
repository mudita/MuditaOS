// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesAgent.hpp"
#include "QuotesQueries.hpp"

//#include <Application.hpp>
#include <Common/Query.hpp>

namespace Quotes
{
    QuotesAgent::QuotesAgent(Database *predefinedQuotesDB,
                             Database *customQuotesDB,
                             std::unique_ptr<settings::Settings> settings)
        : predefinedDB(predefinedQuotesDB), customDB(customQuotesDB),
          randomizedQuoteModel(std::move(settings), predefinedQuotesDB, customQuotesDB)
    {
        constexpr auto forced = false;
        randomizedQuoteModel.updateList(forced);
    }

    auto QuotesAgent::runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        if (typeid(*query) == typeid(Messages::GetCategoryListRequest)) {
            return handleCategoryList(query);
        }
        else if (typeid(*query) == typeid(Messages::EnableCategoryByIdRequest)) {
            return handleEnableCategoryById(query);
        }
        else if (typeid(*query) == typeid(Messages::GetQuotesListFromCustomCategoryRequest)) {
            return handleQuotesListFromCustomCategory(query);
        }
        else if (typeid(*query) == typeid(Messages::EnableQuoteByIdRequest)) {
            return handleEnableQuoteById(query);
        }
        else if (typeid(*query) == typeid(Messages::AddQuoteRequest)) {
            return handleAddQuote(query);
        }
        else if (typeid(*query) == typeid(Messages::ReadRandomizedQuoteRequest)) {
            return handleReadRandomizedQuote(query);
        }
        else if (typeid(*query) == typeid(Messages::InformLanguageChangeRequest)) {
            handleInformLanguageChange();
            return nullptr;
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
        auto queryResult  = predefinedDB->query(Queries::getAllCategories, utils::getDisplayLanguage().c_str());
        auto categoryList = getList<CategoryList, CategoryRecord>(req->offset, req->limit, std::move(queryResult));
        auto response     = std::make_unique<Messages::GetCategoryListResponse>(std::move(categoryList));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleEnableCategoryById(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req              = std::dynamic_pointer_cast<Messages::EnableCategoryByIdRequest>(query);
        auto queryResult      = predefinedDB->execute(Queries::enableCategory, req->enable, req->categoryId);
        auto response         = std::make_unique<Messages::EnableCategoryByIdResponse>(std::move(queryResult));
        constexpr auto forced = true;
        randomizedQuoteModel.updateList(forced);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleQuotesListFromCustomCategory(std::shared_ptr<db::Query> query)
        -> std::unique_ptr<db::QueryResult>
    {
        auto req         = std::dynamic_pointer_cast<Messages::GetQuotesListFromCustomCategoryRequest>(query);
        auto queryResult = customDB->query(Queries::getAllCustomQuotes);
        auto quotesList  = getList<QuotesList, QuoteRecord>(req->offset, req->limit, std::move(queryResult));
        auto response    = std::make_unique<Messages::GetQuotesListFromCustomCategoryResponse>(std::move(quotesList));
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleEnableQuoteById(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req              = std::dynamic_pointer_cast<Messages::EnableQuoteByIdRequest>(query);
        auto queryResult      = customDB->execute(Queries::enableQuote, req->enable, req->quoteId);
        auto response         = std::make_unique<Messages::EnableQuoteByIdResponse>(queryResult);
        constexpr auto forced = true;
        randomizedQuoteModel.updateList(forced);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleAddQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req = std::dynamic_pointer_cast<Messages::AddQuoteRequest>(query);

        auto success = customDB->execute(Queries::addQuote, req->quote.c_str(), req->author.c_str(), req->enabled);

        if (success) {
            constexpr auto forced = true;
            randomizedQuoteModel.updateList(forced);
        }
        auto quoteId  = customDB->getLastInsertRowId();
        auto response = std::make_unique<Messages::AddQuoteResponse>(success, quoteId);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleReadRandomizedQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req                  = std::dynamic_pointer_cast<Messages::ReadRandomizedQuoteRequest>(query);
        auto [type, randomizedId] = randomizedQuoteModel.getId();
        LOG_DEBUG("Randomized id: %d, type: %d", randomizedId, static_cast<int>(type));
        std::unique_ptr<QueryResult> queryResult;

        if (type == QuoteType::Predefined) {
            queryResult = predefinedDB->query(Queries::readPredefinedQuote, randomizedId);
        }
        else {
            queryResult = customDB->query(Queries::readCustomQuote, randomizedId);
        }

        if (queryResult->getRowCount()) {
            QuoteRecord quoteRecord(queryResult.get());
            auto response = std::make_unique<Messages::ReadRandomizedQuoteResponse>(
                quoteRecord.quote_id, quoteRecord.quote, quoteRecord.author);
            response->setRequestQuery(query);
            return response;
        }
        else {
            return nullptr;
        }
    }

    void QuotesAgent::handleInformLanguageChange()
    {
        constexpr auto forced = true;
        randomizedQuoteModel.updateList(forced);
    }

    auto QuotesAgent::handleWriteQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req = std::dynamic_pointer_cast<Messages::WriteQuoteRequest>(query);
        auto queryResult =
            customDB->execute(Queries::writeQuote, req->quote.c_str(), req->author.c_str(), req->enabled, req->quoteId);
        constexpr auto forced = true;
        randomizedQuoteModel.updateList(forced);
        auto response = std::make_unique<Messages::WriteQuoteResponse>(queryResult);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleDeleteQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        auto req              = std::dynamic_pointer_cast<Messages::DeleteQuoteRequest>(query);
        auto queryResult      = customDB->execute(Queries::deleteQuote, req->quoteId);
        constexpr auto forced = true;
        randomizedQuoteModel.updateList(forced);
        auto response = std::make_unique<Messages::DeleteQuoteResponse>(queryResult);
        response->setRequestQuery(query);
        return response;
    }

} // namespace Quotes
