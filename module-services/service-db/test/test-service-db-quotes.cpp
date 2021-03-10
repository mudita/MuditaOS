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
        QuotesList quotesList;
        quotesList.limit  = 3;
        quotesList.offset = 4;

        auto record  = std::make_unique<QuotesList>(quotesList);
        auto request = std::make_shared<Messages::GetQuotesListRequest>(std::move(record));
        auto response =
            std::dynamic_pointer_cast<Messages::GetQuotesListResponse>(tester->handleQuotesList(request.get()));

        REQUIRE(response->getCount() == totalNumOfQuotesInDb);
        REQUIRE(response->getResults().size() == quotesList.limit);
    }

    SECTION("Get all quotes")
    {
        QuotesList quotesList;
        quotesList.limit = 0;

        auto record  = std::make_unique<QuotesList>(quotesList);
        auto request = std::make_shared<Messages::GetQuotesListRequest>(std::move(record));
        auto response =
            std::dynamic_pointer_cast<Messages::GetQuotesListResponse>(tester->handleQuotesList(request.get()));

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

    Database::deinitialize();
}
