// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/QuotesMessages.hpp>
#include <service-db/agents/quotes/QuotesAgent.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace Quotes
{
    class QuotesAgentTester : public QuotesAgent
    {
      public:
        QuotesAgentTester(Database *predefinedDB, Database *customDB, std::unique_ptr<settings::Settings> settings)
            : QuotesAgent(predefinedDB, customDB, std::move(settings)){};
        ~QuotesAgentTester() = default;

        auto getCategoriesList() -> std::vector<CategoryRecord>
        {
            unsigned int offset = 0;
            unsigned int limit  = 0;

            auto request     = std::make_shared<Messages::GetCategoryListRequest>(offset, limit);
            auto queryResult = handleCategoryList(request);
            auto response    = dynamic_cast<Messages::GetCategoryListResponse *>(queryResult.get());
            return response->getResults();
        }

        auto getQuotesFromCustomCategory() -> std::vector<QuoteRecord>
        {
            unsigned int offset = 0;
            unsigned int limit  = 0;

            auto request     = std::make_shared<Messages::GetQuotesListFromCustomCategoryRequest>(offset, limit);
            auto queryResult = handleQuotesListFromCustomCategory(request);
            auto response    = dynamic_cast<Messages::GetQuotesListFromCustomCategoryResponse *>(queryResult.get());
            return response->getResults();
        }

        auto enableCategory(unsigned int categoryId, bool enable) -> bool
        {
            auto request     = std::make_shared<Messages::EnableCategoryByIdRequest>(categoryId, enable);
            auto queryResult = handleEnableCategoryById(request);
            auto response    = dynamic_cast<Messages::EnableCategoryByIdResponse *>(queryResult.get());
            return response->success;
        }

        auto enableQuote(unsigned int quoteId, bool enable) -> bool
        {
            auto request     = std::make_shared<Messages::EnableQuoteByIdRequest>(quoteId, enable);
            auto queryResult = handleEnableQuoteById(request);
            auto response    = dynamic_cast<Messages::EnableQuoteByIdResponse *>(queryResult.get());
            return response->success;
        }

        auto addQuote(std::string quote, std::string author, bool enabled) -> unsigned int
        {
            auto request = std::make_shared<Messages::AddQuoteRequest>(std::move(quote), std::move(author), enabled);
            auto queryResult = handleAddQuote(request);
            auto response    = dynamic_cast<Messages::AddQuoteResponse *>(queryResult.get());
            return response->quoteId;
        }

        auto readRandomizedQuote() -> std::unique_ptr<db::QueryResult>
        {
            auto request = std::make_shared<Messages::ReadRandomizedQuoteRequest>();
            return handleReadRandomizedQuote(request);
        }

        auto informLanguageChange() -> void
        {
            handleInformLanguageChange();
        }

        auto writeQuote(unsigned int quoteId, std::string quote, std::string author, bool enabled) -> bool
        {
            auto request =
                std::make_shared<Messages::WriteQuoteRequest>(quoteId, std::move(quote), std::move(author), enabled);
            auto queryResult = handleWriteQuote(request);
            auto response    = dynamic_cast<Messages::WriteQuoteResponse *>(queryResult.get());
            return response->success;
        }

        auto deleteQuote(unsigned int quoteId) -> bool
        {
            auto request     = std::make_shared<Messages::DeleteQuoteRequest>(quoteId);
            auto queryResult = handleDeleteQuote(request);
            auto response    = dynamic_cast<Messages::DeleteQuoteResponse *>(queryResult.get());
            return response->success;
        }
    };

    class SettingsMock : public settings::Settings
    {
      public:
        SettingsMock(std::string &quotesStr, std::string &timestampStr)
            : quotesString(quotesStr), timestampString(timestampStr)
        {}
        void setValue(const std::string &variableName,
                      const std::string &variableValue,
                      settings::SettingsScope scope = settings::SettingsScope::AppLocal)
        {
            if (variableName == settings::Quotes::randomQuotesList) {
                quotesString = variableValue;
            }
            else if (variableName == settings::Quotes::randomQuoteIDUpdateTime) {
                timestampString = variableValue;
            }
        }
        std::string getValue(const std::string &variableName,
                             settings::SettingsScope scope = settings::SettingsScope::AppLocal)
        {
            if (variableName == settings::Quotes::randomQuotesList) {
                return quotesString;
            }
            else if (variableName == settings::Quotes::randomQuoteIDUpdateTime) {
                return timestampString;
            }
            else {
                return std::string();
            }
        }
        std::string &quotesString;
        std::string &timestampString;
    };

} // namespace Quotes
