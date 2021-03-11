// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "test-service-db-quotes.hpp"

using namespace Quotes;

constexpr auto totalNumOfCategoriesInDb = 6;
constexpr auto totalNumOfQuotesInDb     = 48;
constexpr auto numOfQuotesWithCategoryIdEqualToOne = 9;

TEST_CASE("Quotes")
{
    Database::initialize();

    auto tester = std::make_unique<QuotesAgentTester>(nullptr);

    SECTION("Get categories with limit and offset")
    {
        CategoryList categoryList;
        categoryList.limit  = 2;
        categoryList.offset = 1;

        auto record  = std::make_unique<CategoryList>(categoryList);
        auto request = std::make_shared<Messages::GetCategoryListRequest>(std::move(record));
        auto response =
            std::dynamic_pointer_cast<Messages::GetCategoryListResponse>(tester->handleCategoryList(request.get()));

        REQUIRE(response->getCount() == totalNumOfCategoriesInDb);
        REQUIRE(response->getResults().size() == categoryList.limit);
    }

    SECTION("Get all categories")
    {
        CategoryList categoryList;
        categoryList.limit = 0;

        auto record  = std::make_unique<CategoryList>(categoryList);
        auto request = std::make_shared<Messages::GetCategoryListRequest>(std::move(record));
        auto response =
            std::dynamic_pointer_cast<Messages::GetCategoryListResponse>(tester->handleCategoryList(request.get()));

        REQUIRE(response->getCount() == totalNumOfCategoriesInDb);
        REQUIRE(response->getResults().size() == totalNumOfCategoriesInDb);
    }

    SECTION("Get quotes with limit and offset")
    {
        unsigned int limit  = 3;
        unsigned int offset = 4;

        auto response = tester->getAllQuotes(limit, offset);

        REQUIRE(response->getCount() == totalNumOfQuotesInDb);
        REQUIRE(response->getResults().size() == limit);
    }

    SECTION("Get all quotes")
    {
        auto response = tester->getAllQuotes();

        REQUIRE(response->getCount() == totalNumOfQuotesInDb);
        REQUIRE(response->getResults().size() == totalNumOfQuotesInDb);
    }

    SECTION("Get quotes by category id")
    {
        const unsigned int categoryId = 1;
        auto quotes                   = tester->getQuotesByCategoryId(categoryId);
        REQUIRE(quotes.size() == numOfQuotesWithCategoryIdEqualToOne);
    }

    SECTION("Enable category by id")
    {
        bool enable                   = false;
        const unsigned int categoryId = 1;

        auto response = tester->enableCategory(categoryId, enable);
        REQUIRE(response->success);

        // Quotes in category one should be disabled
        auto quotes = tester->getQuotesByCategoryId(categoryId);
        REQUIRE(quotes.size() == 0);

        enable = true;

        response = tester->enableCategory(categoryId, enable);
        REQUIRE(response->success);

        // Quotes in category one should be enabled
        quotes = tester->getQuotesByCategoryId(categoryId);
        REQUIRE(quotes.size() == numOfQuotesWithCategoryIdEqualToOne);
    }

    SECTION("Enable quote by id")
    {
        bool enable                = false;
        const unsigned int quoteId = 1;

        auto response = tester->enableQuote(quoteId, enable);
        REQUIRE(response->success);

        // All quotes except quote with id=1 should be enabled
        auto quotes = tester->getEnabledQuotes();
        REQUIRE(quotes.size() == totalNumOfQuotesInDb - 1);

        enable = true;

        response = tester->enableQuote(quoteId, enable);
        REQUIRE(response->success);

        // All quotes should be enabled
        quotes = tester->getEnabledQuotes();
        REQUIRE(quotes.size() == totalNumOfQuotesInDb);
    }

    SECTION("Add/Read/Write/Delete quote")
    {
        unsigned int langId = 1;
        std::string quote   = "TEST QUOTE";
        std::string author  = "TEST AUTHOR";
        bool enabled        = true;

        // Add a new quote
        auto addQuoteResponse = tester->addQuote(langId, quote, author, enabled);
        REQUIRE(addQuoteResponse->success);
        const auto quoteId = addQuoteResponse->quoteId;

        // Check if quotes count has increased
        auto getAllQuotesResponse = tester->getAllQuotes();

        REQUIRE(getAllQuotesResponse->getCount() == totalNumOfQuotesInDb + 1);
        REQUIRE(getAllQuotesResponse->getResults().size() == totalNumOfQuotesInDb + 1);

        // Read added quote
        auto readQuoteResponse = tester->readQuote(quoteId);

        REQUIRE(readQuoteResponse->quoteId == quoteId);
        REQUIRE(readQuoteResponse->quote == quote);
        REQUIRE(readQuoteResponse->author == author);

        // Change added quote (overwrite)
        quote                   = "TEST QUOTE CHANGED";
        author                  = "TEST AUTHOR CHANGED";
        auto writeQuoteResponse = tester->writeQuote(quoteId, langId, quote, author, enabled);
        REQUIRE(writeQuoteResponse->success);

        // Read quote if values have been properly updated
        readQuoteResponse = tester->readQuote(quoteId);

        REQUIRE(readQuoteResponse->quoteId == quoteId);
        REQUIRE(readQuoteResponse->quote == quote);
        REQUIRE(readQuoteResponse->author == author);

        // Delete added quote
        auto deleteQuoteResponse = tester->deleteQuote(quoteId);
        REQUIRE(deleteQuoteResponse->success);

        // Check if quotes count match count before added quote
        getAllQuotesResponse = tester->getAllQuotes();

        REQUIRE(getAllQuotesResponse->getCount() == totalNumOfQuotesInDb);
        REQUIRE(getAllQuotesResponse->getResults().size() == totalNumOfQuotesInDb);
    }

    Database::deinitialize();
}
