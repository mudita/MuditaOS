// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "test-service-db-quotes.hpp"
#include <purefs/filesystem_paths.hpp>
#include <i18n/i18n.hpp>

#include <iostream>

using namespace Quotes;

constexpr auto totalNumOfCategories          = 3;
constexpr auto totalNumOfQuotes              = 5;

TEST_CASE("Quotes")
{
    Database::initialize();
    auto predefinedDB =
        std::make_unique<Database>((purefs::dir::getUserDiskPath() / "predefined_quotes.db").string().c_str());
    auto customDB = std::make_unique<Database>((purefs::dir::getUserDiskPath() / "custom_quotes.db").string().c_str());
    std::string timestampString{};
    std::string quotesString{};
    auto settings = std::make_unique<Quotes::SettingsMock>(quotesString, timestampString);
    auto tester   = std::make_unique<QuotesAgentTester>(predefinedDB.get(), customDB.get(), std::move(settings));

    SECTION("Get all categories")
    {
        utils::setDisplayLanguage("English");
        tester->informLanguageChange();
        auto categories = tester->getCategoriesList();
        REQUIRE(categories.size() == totalNumOfCategories);
    }

    SECTION("Get quotes from custom category")
    {
        auto quotes = tester->getQuotesFromCustomCategory();
        REQUIRE(quotes.size() > 0);
    }

    SECTION("Enable category by id")
    {
        bool enable;
        const unsigned int categoryId = 5;

        // Initial conditions set
        auto categories = tester->getCategoriesList();
        for (const auto &category : categories) {
            if (category.category_id == categoryId) {
                enable = category.enabled;
            }
        }

        // For quotes randomizer checks
        auto oldRandomizedSequence = quotesString;

        auto success = tester->enableCategory(categoryId, !enable);
        REQUIRE(success);
        categories = tester->getCategoriesList();
        for (const auto &category : categories) {
            if (category.category_id == categoryId) {
                REQUIRE(category.enabled != enable);
            }
        }
        REQUIRE(oldRandomizedSequence != quotesString);

        oldRandomizedSequence = quotesString;
        enable = true;
        success = tester->enableCategory(categoryId, enable);
        REQUIRE(success);
        categories = tester->getCategoriesList();
        for (const auto &category : categories) {
            if (category.category_id == categoryId) {
                REQUIRE(enable);
            }
        }

        REQUIRE(oldRandomizedSequence != quotesString);
    }

    SECTION("Enable quote by id")
    {
        bool enable                = false;
        const unsigned int quoteId = 1;

        // For quotes randomizer checks
        auto oldRandomizedSequence = quotesString;

        auto success = tester->enableQuote(quoteId, enable);
        REQUIRE(success);

        auto customQuotes = tester->getQuotesFromCustomCategory();
        for (const auto &quote : customQuotes) {
            if (quote.quote_id == quoteId) {
                REQUIRE(!quote.enabled);
            }
        }
        REQUIRE(oldRandomizedSequence != quotesString);

        oldRandomizedSequence = quotesString;
        enable = true;
        success = tester->enableQuote(quoteId, enable);
        REQUIRE(success);

        customQuotes = tester->getQuotesFromCustomCategory();
        for (const auto &quote : customQuotes) {
            if (quote.quote_id == quoteId) {
                REQUIRE(quote.enabled);
            }
        }
        REQUIRE(oldRandomizedSequence != quotesString);
    }

    SECTION("Add/Read/Write/Delete quote")
    {
        std::string quote   = "TEST QUOTE";
        std::string author  = "TEST AUTHOR";
        bool enabled        = true;

        // Initial conditions
        auto customQuotes                  = tester->getQuotesFromCustomCategory();
        auto numOfQuotesFromCustomCategory = customQuotes.size();

        // For quotes randomizer checks
        auto oldRandomizedSequence = quotesString;

        // Add a new quote
        auto quoteId = tester->addQuote(quote, author, enabled);
        REQUIRE(oldRandomizedSequence != quotesString);

        // Check if quotes count has increased
        customQuotes = tester->getQuotesFromCustomCategory();
        REQUIRE(customQuotes.size() == numOfQuotesFromCustomCategory + 1);

        // Read added quote
        for (const auto &customQuote : customQuotes) {
            if (customQuote.quote_id == quoteId) {
                REQUIRE(customQuote.quote == quote);
                REQUIRE(customQuote.author == author);
            }
        }

        // Change added quote (overwrite)
        quote        = "TEST QUOTE CHANGED";
        author       = "TEST AUTHOR CHANGED";
        auto success = tester->writeQuote(quoteId, quote, author, enabled);
        REQUIRE(success);

        // Read quote if values have been properly updated
        customQuotes = tester->getQuotesFromCustomCategory();
        for (const auto &customQuote : customQuotes) {
            if (customQuote.quote_id == quoteId) {
                REQUIRE(customQuote.quote == quote);
                REQUIRE(customQuote.author == author);
            }
        }

        oldRandomizedSequence = quotesString;
        // Delete added quote
        success = tester->deleteQuote(quoteId);
        REQUIRE(success);
        REQUIRE(oldRandomizedSequence != quotesString);

        // Check if quotes count match count before added quote
        customQuotes = tester->getQuotesFromCustomCategory();
        REQUIRE(customQuotes.size() == numOfQuotesFromCustomCategory);
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

    SECTION("Randomizer test - no enabled quotes")
    {
        timestampString = std::to_string(0);
        auto categories = tester->getCategoriesList();

        // disable all quotes
        for (const auto &category : categories) {
            tester->enableCategory(category.category_id, false);
        }

        auto customQuotes = tester->getQuotesFromCustomCategory();
        for (const auto &quote : customQuotes) {
            tester->enableQuote(quote.quote_id, false);
        }

        // No crash expected
        REQUIRE_NOTHROW(tester->readRandomizedQuote());
    }

    Database::deinitialize();
}

TEST_CASE("Serializer test")
{

    QuotesSettingsSerializer serializer;

    SECTION("Serialize - correct input")
    {
        IdList list{
            {QuoteType::Predefined, 1}, {QuoteType::Custom, 2}, {QuoteType::Predefined, 3}, {QuoteType::Custom, 4}};
        auto result = serializer.serialize(list);
        REQUIRE(result == "0,1,1,2,0,3,1,4,");

        IdList list2{{QuoteType::Custom, 1}};
        result = serializer.serialize(list2);
        REQUIRE(result == "1,1,");
    }

    SECTION("Serialize - wrong input")
    {
        IdList list{};
        auto result = serializer.serialize(list);
        REQUIRE(result.empty());
    }

    SECTION("Deserialize - correct input")
    {
        std::string serializedData = "0,1,1,2,0,3,1,4,";
        auto result                = serializer.deserialize(serializedData);
        REQUIRE(result.size() == 4);
        REQUIRE(result.front().first == QuoteType::Predefined);
        REQUIRE(result.front().second == 1);
        REQUIRE(result.back().first == QuoteType::Custom);
        REQUIRE(result.back().second == 4);
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
