// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
    std::string timestampString{};
    std::string quotesString{};
    auto settings = std::make_unique<Quotes::SettingsMock>(quotesString, timestampString);
    auto tester   = std::make_unique<QuotesAgentTester>(database.get(), std::move(settings));

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

        // Get current random quote
        auto queryResult   = tester->readRandomizedQuote();
        auto randomQuoteId = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get())->quoteId;
        REQUIRE(randomQuoteId != 0);

        auto success = tester->enableCategory(categoryId, enable);
        REQUIRE(success);

        // Quotes in category one should be disabled
        auto quotes = tester->getQuotesByCategoryId(categoryId);
        REQUIRE(quotes.size() == 0);

        // verify rerandomizing the quotes list
        queryResult           = tester->readRandomizedQuote();
        auto newRandomQuoteId = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get())->quoteId;
        REQUIRE(newRandomQuoteId != 0);
        REQUIRE(randomQuoteId != newRandomQuoteId);

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

        // Get current random quote
        auto queryResult   = tester->readRandomizedQuote();
        auto randomQuoteId = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get())->quoteId;
        REQUIRE(randomQuoteId != 0);

        auto success = tester->enableQuote(quoteId, enable);
        REQUIRE(success);

        // All quotes except quote with id=1 should be enabled
        auto quotes = tester->getEnabledQuotes();
        REQUIRE(quotes.size() == totalNumOfQuotesInDb - 1);

        // verify rerandomizing the quotes list
        queryResult           = tester->readRandomizedQuote();
        auto newRandomQuoteId = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get())->quoteId;
        REQUIRE(newRandomQuoteId != 0);
        REQUIRE(randomQuoteId != newRandomQuoteId);

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

        // Get current random quote
        auto queryResult   = tester->readRandomizedQuote();
        auto randomQuoteId = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get())->quoteId;
        REQUIRE(randomQuoteId != 0);

        // Add a new quote
        auto quoteId = tester->addQuote(langId, quote, author, enabled);

        // Check if quotes count has increased
        auto quotes = tester->getAllQuotes();

        REQUIRE(quotes.size() == totalNumOfQuotesInDb + 1);

        // verify rerandomizing the quotes list
        queryResult           = tester->readRandomizedQuote();
        auto newRandomQuoteId = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get())->quoteId;
        REQUIRE(newRandomQuoteId != 0);
        REQUIRE(randomQuoteId != newRandomQuoteId);

        // Read added quote
        queryResult            = tester->readQuote(quoteId);
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

    SECTION("Randomizer test - double request")
    {

        auto queryResult = tester->readRandomizedQuote();
        auto response    = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get());

        REQUIRE(response);
        auto quoteId = response->quoteId;

        queryResult = tester->readRandomizedQuote();
        response    = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get());
        REQUIRE(response);
        auto newQuoteId = response->quoteId;

        REQUIRE(quoteId == newQuoteId);
    }
    SECTION("Randomizer test - time shift correct")
    {
        constexpr auto timeshift24h = 24 * 3600;

        auto queryResult = tester->readRandomizedQuote();
        auto response    = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get());
        REQUIRE(response);
        auto quoteId = response->quoteId;

        timestampString = std::to_string(std::time(nullptr) - timeshift24h);
        queryResult     = tester->readRandomizedQuote();
        response        = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get());
        REQUIRE(response);
        auto newQuoteId = response->quoteId;

        REQUIRE(quoteId != newQuoteId);
    }
    SECTION("Randomizer test - time shift wrong")
    {
        constexpr auto timeshift23h = 23 * 3600;

        auto queryResult = tester->readRandomizedQuote();
        auto response    = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get());
        REQUIRE(response);
        auto quoteId = response->quoteId;

        timestampString = std::to_string(std::time(nullptr) - timeshift23h);
        queryResult     = tester->readRandomizedQuote();
        response        = dynamic_cast<Messages::ReadRandomizedQuoteResponse *>(queryResult.get());
        REQUIRE(response);
        auto newQuoteId = response->quoteId;

        REQUIRE(quoteId == newQuoteId);
    }

    Database::deinitialize();
}

TEST_CASE("Serializer test")
{

    QuotesSettingsSerializer serializer;

    SECTION("Serialize - correct input")
    {
        IdList list{1, 2, 3, 4};
        auto result = serializer.serialize(list);
        REQUIRE(result == "1,2,3,4,");

        IdList list2{1};
        result = serializer.serialize(list2);
        REQUIRE(result == "1,");
    }

    SECTION("Serialize - wrong input")
    {
        IdList list{};
        auto result = serializer.serialize(list);
        REQUIRE(result.empty());
    }

    SECTION("Deserialize - correct input")
    {
        std::string serializedData = "1,2,3,4,";
        auto result                = serializer.deserialize(serializedData);
        REQUIRE(result.size() == 4);
        REQUIRE(result.front() == 1);
        REQUIRE(result.back() == 4);
    }

    SECTION("Deserialize - empty input")
    {
        std::string serializedData = " ";
        IdList result;
        REQUIRE_NOTHROW(result = serializer.deserialize(serializedData));
        REQUIRE(result.empty());
    }

    SECTION("Deserialize - wrong input 1/3")
    {
        std::string serializedData = "abc";
        IdList result;
        REQUIRE_NOTHROW(result = serializer.deserialize(serializedData));
        REQUIRE(result.empty());
    }

    SECTION("Deserialize - wrong input 2/3")
    {
        std::string serializedData = "a,b,c";
        IdList result;
        REQUIRE_NOTHROW(result = serializer.deserialize(serializedData));
        REQUIRE(result.empty());
    }

    SECTION("Deserialize - wrong input 3/3")
    {
        std::string serializedData = ",1,2,3,4";
        IdList result;
        REQUIRE_NOTHROW(result = serializer.deserialize(serializedData));
        REQUIRE(result.empty());
    }
}
