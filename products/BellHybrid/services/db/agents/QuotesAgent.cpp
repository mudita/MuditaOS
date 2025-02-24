// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QuotesAgent.hpp"
#include "QuotesQueries.hpp"
#include <Common/Query.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace Quotes
{
    QuotesAgent::QuotesAgent(Database *quotesDB, std::unique_ptr<settings::Settings> settings)
        : settings(std::move(settings)), quotesDB(quotesDB), shuffleQuoteModel(*this->settings, quotesDB)
    {
        shuffleQuoteModel.updateList(ListUpdateMode::Normal);
    }

    auto QuotesAgent::runQuery(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        if (typeid(*query) == typeid(Messages::ReadRandomizedQuoteRequest)) {
            return handleReadRandomizedQuote(query);
        }
        else if (typeid(*query) == typeid(Messages::InformLanguageChangeRequest) ||
                 (typeid(*query) == typeid(Messages::InformDateChanged))) {
            shuffleQuoteModel.updateList(ListUpdateMode::Forced);
            return std::make_unique<Messages::NotificationResult>(true);
        }
        else if (typeid(*query) == typeid(Messages::AddNewEntry)) {
            return handleAddNewEntry(query);
        }
        else if (typeid(*query) == typeid(Messages::EditEntry)) {
            return handleEditEntry(query);
        }
        else if (typeid(*query) == typeid(Messages::DeleteQuoteRequest)) {
            return handleDeleteEntry(query);
        }
        else if (typeid(*query) == typeid(Messages::InformGroupChanged)) {
            return handleGroupChanged(query);
        }
        else if (typeid(*query) == typeid(Messages::InformIntervalChanged)) {
            return handleIntervalChanged(query);
        }
        return nullptr;
    }

    auto QuotesAgent::isCustomQuotesDatabaseEmpty() -> bool
    {
        const auto result = quotesDB->query(Queries::getCustomQuotesCount);
        if (result == nullptr || result->getRowCount() == 0) {
            return true;
        }
        return ((*result)[0].getUInt32() == 0);
    }

    auto QuotesAgent::handleReadRandomizedQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        const auto request = std::dynamic_pointer_cast<Messages::ReadRandomizedQuoteRequest>(query);
        if (request) {
            const auto id = shuffleQuoteModel.getId();
            const auto quotesGroup =
                settings->getValue(settings::Quotes::selectedGroup, settings::SettingsScope::Global);
            LOG_DEBUG("Shuffle id: %d", id);
            const auto readQuery = quotesGroup == customGroup ? Queries::readCustomQuote : Queries::readPredefinedQuote;
            const auto result    = quotesDB->query(readQuery, id);
            if (result && result->getRowCount() > 0) {
                const IdQuoteAuthorRecord record(result.get());
                auto response = std::make_unique<Messages::ReadRandomizedQuoteResponse>(
                    record.quote_id, record.quote, record.author);
                response->setRequestQuery(query);
                return response;
            }
        }
        return nullptr;
    }

    auto QuotesAgent::handleAddNewEntry(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        const auto request = std::dynamic_pointer_cast<Messages::AddNewEntry>(query);
        if (request == nullptr) {
            auto response = std::make_unique<Messages::AddQuoteResponse>();
            response->setRequestQuery(query);
            return response;
        }

        const auto quotesGroup = settings->getValue(settings::Quotes::selectedGroup, settings::SettingsScope::Global);
        const auto executeResult =
            quotesDB->execute(Queries::addCustomQuote, request->quote.c_str(), request->author.c_str());

        if (executeResult && quotesGroup == customGroup) {
            shuffleQuoteModel.updateList(ListUpdateMode::Forced);
        }

        const auto quoteId = quotesDB->getLastInsertRowId();
        auto response      = std::make_unique<Messages::AddQuoteResponse>(executeResult, quoteId);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleEditEntry(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        const auto request = std::dynamic_pointer_cast<Messages::EditEntry>(query);
        if (request == nullptr) {
            auto response = std::make_unique<Messages::EditEntryResponse>();
            response->setRequestQuery(query);
            return response;
        }

        const auto quotesGroup = settings->getValue(settings::Quotes::selectedGroup, settings::SettingsScope::Global);
        const auto executeResult =
            quotesDB->execute(Queries::editCustomQuote, request->quote.c_str(), request->author.c_str(), request->id);

        if (executeResult && quotesGroup == customGroup) {
            shuffleQuoteModel.updateList(ListUpdateMode::Forced);
        }

        auto response = std::make_unique<Messages::EditEntryResponse>(executeResult);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleDeleteEntry(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        const auto request = std::dynamic_pointer_cast<Messages::DeleteQuoteRequest>(query);
        if (request == nullptr) {
            auto response = std::make_unique<Messages::DeleteQuoteResponse>();
            response->setRequestQuery(query);
            return response;
        }

        const auto quotesGroup   = settings->getValue(settings::Quotes::selectedGroup, settings::SettingsScope::Global);
        const auto executeResult = quotesDB->execute(Queries::deleteCustomQuote, request->quoteId);

        if (executeResult && quotesGroup == customGroup) {
            if (isCustomQuotesDatabaseEmpty()) {
                // if the list of custom quotes is empty, we change the group to a predefined one
                settings->setValue(settings::Quotes::selectedGroup, predefinedGroup, settings::SettingsScope::Global);
            }
            shuffleQuoteModel.updateList(ListUpdateMode::Forced);
        }

        auto response = std::make_unique<Messages::DeleteQuoteResponse>(executeResult);
        response->setRequestQuery(query);
        return response;
    }

    auto QuotesAgent::handleGroupChanged(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        const auto request = std::dynamic_pointer_cast<Messages::InformGroupChanged>(query);
        if (request == nullptr) {
            return std::make_unique<Messages::NotificationResult>(false);
        }
        if (request->group == customGroup && isCustomQuotesDatabaseEmpty()) {
            // if the list of custom quotes is empty, we change the group to a predefined one
            settings->setValue(settings::Quotes::selectedGroup, predefinedGroup, settings::SettingsScope::Global);
            shuffleQuoteModel.updateList(ListUpdateMode::Forced);
            return std::make_unique<Messages::NotificationResult>(true);
        }

        settings->setValue(settings::Quotes::selectedGroup, request->group, settings::SettingsScope::Global);
        shuffleQuoteModel.updateList(ListUpdateMode::Forced);
        return std::make_unique<Messages::NotificationResult>(true);
    }

    auto QuotesAgent::handleIntervalChanged(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        const auto request = std::dynamic_pointer_cast<Messages::InformIntervalChanged>(query);
        if (request == nullptr) {
            return std::make_unique<Messages::NotificationResult>(false);
        }
        if (!utils::is_number(request->interval) && request->interval != atMidnight) {
            return std::make_unique<Messages::NotificationResult>(false);
        }

        settings->setValue(settings::Quotes::selectedInterval, request->interval, settings::SettingsScope::Global);
        shuffleQuoteModel.updateList(ListUpdateMode::Forced);
        return std::make_unique<Messages::NotificationResult>(true);
    }
} // namespace Quotes
