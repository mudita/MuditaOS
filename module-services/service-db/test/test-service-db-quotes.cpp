// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "test-service-db-quotes.hpp"

#include <purefs/filesystem_paths.hpp>
#include <iostream>

using namespace Quotes;

constexpr auto totalNumOfCategoriesInDb = 6;
constexpr auto totalNumOfQuotesInDb     = 48;
constexpr auto numOfQuotesFromCustomCategory       = 6;
constexpr auto numOfQuotesWithCategoryIdEqualToOne = 9;

TEST_CASE("Quotes")
{
    Database::initialize();
    auto database = std::make_unique<Database>((purefs::dir::getUserDiskPath() / "quotes.db").string().c_str());
    auto tester   = std::make_unique<QuotesAgentTester>(database.get());

    SECTION("Get categories with limit and offset")
    {
        unsigned int offset = 1;
        unsigned int limit  = 2;

        auto request     = std::make_shared<Messages::GetCategoryListRequest>(offset, limit);
        auto queryResult = tester->handleCategoryList(request);
        auto response    = dynamic_cast<Messages::GetCategoryListResponse *>(queryResult.get());

        REQUIRE(response);
        REQUIRE(response->getCount() == totalNumOfCategoriesInDb);
        REQUIRE(response->getResults().size() == limit);
    }

    SECTION("Get all categories")
    {
        unsigned int offset = 0;
        unsigned int limit  = 0;

        auto request     = std::make_shared<Messages::GetCategoryListRequest>(offset, limit);
        auto queryResult = tester->handleCategoryList(request);
        auto response    = dynamic_cast<Messages::GetCategoryListResponse *>(queryResult.get());

        REQUIRE(response);
        REQUIRE(response->getCount() == totalNumOfCategoriesInDb);
        REQUIRE(response->getResults().size() == totalNumOfCategoriesInDb);
    }

    SECTION("Get quotes from custom category")
    {
        auto quotes = tester->getQuotesFromCustomCategory();
        REQUIRE(quotes.size() == numOfQuotesFromCustomCategory);
    }

    SECTION("Enable category by id")
    {
        bool enable                   = false;
        const unsigned int categoryId = 1;

        auto success = tester->enableCategory(categoryId, enable);
        REQUIRE(success);

        // Quotes in category one should be disabled
        auto quotes = tester->getQuotesByCategoryId(categoryId);
        REQUIRE(quotes.size() == 0);

        enable = true;

        success = tester->enableCategory(categoryId, enable);
        REQUIRE(success);

        // Quotes in category one should be enabled
        quotes = tester->getQuotesByCategoryId(categoryId);
        REQUIRE(quotes.size() == numOfQuotesWithCategoryIdEqualToOne);
    }

    SECTION("Enable quote by id")
    {
        bool enable                = false;
        const unsigned int quoteId = 1;

        auto success = tester->enableQuote(quoteId, enable);
        REQUIRE(success);

        // All quotes except quote with id=1 should be enabled
        auto quotes = tester->getEnabledQuotes();
        REQUIRE(quotes.size() == totalNumOfQuotesInDb - 1);

        enable = true;

        success = tester->enableQuote(quoteId, enable);
        REQUIRE(success);

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
        auto quoteId = tester->addQuote(langId, quote, author, enabled);

        // Check if quotes count has increased
        auto quotes = tester->getAllQuotes();

        REQUIRE(quotes.size() == totalNumOfQuotesInDb + 1);

        // Read added quote
        auto queryResult       = tester->readQuote(quoteId);
        auto readQuoteResponse = dynamic_cast<Messages::ReadQuoteResponse *>(queryResult.get());

        REQUIRE(readQuoteResponse->quoteId == quoteId);
        REQUIRE(readQuoteResponse->quote == quote);
        REQUIRE(readQuoteResponse->author == author);

        // Change added quote (overwrite)
        quote        = "TEST QUOTE CHANGED";
        author       = "TEST AUTHOR CHANGED";
        auto success = tester->writeQuote(quoteId, langId, quote, author, enabled);
        REQUIRE(success);

        // Read quote if values have been properly updated
        queryResult       = tester->readQuote(quoteId);
        readQuoteResponse = dynamic_cast<Messages::ReadQuoteResponse *>(queryResult.get());

        REQUIRE(readQuoteResponse->quoteId == quoteId);
        REQUIRE(readQuoteResponse->quote == quote);
        REQUIRE(readQuoteResponse->author == author);

        // Delete added quote
        success = tester->deleteQuote(quoteId);
        REQUIRE(success);

        // Check if quotes count match count before added quote
        quotes = tester->getAllQuotes();

        REQUIRE(quotes.size() == totalNumOfQuotesInDb);
    }

    Database::deinitialize();
}
