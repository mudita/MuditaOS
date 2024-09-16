// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "QuotesAgent.hpp"
#include "QuotesQueries.hpp"
#include <Common/Query.hpp>

namespace Quotes
{
    QuotesAgent::QuotesAgent(Database *quotesDB, std::unique_ptr<settings::Settings> settings)
        : quotesDB(quotesDB), shuffleQuoteModel(std::move(settings), quotesDB)
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
        }
        return nullptr;
    }

    auto QuotesAgent::handleReadRandomizedQuote(std::shared_ptr<db::Query> query) -> std::unique_ptr<db::QueryResult>
    {
        const auto request = std::dynamic_pointer_cast<Messages::ReadRandomizedQuoteRequest>(query);
        if (request) {
            const auto id = shuffleQuoteModel.getId();
            LOG_DEBUG("Shuffle id: %d", id);
            const auto result = quotesDB->query(Queries::readQuote, id);
            if (result->getRowCount() > 0) {
                const IdQuoteAuthorRecord record(result.get());
                auto response = std::make_unique<Messages::ReadRandomizedQuoteResponse>(
                    record.quote_id, record.quote, record.author);
                response->setRequestQuery(query);
                return response;
            }
        }
        return nullptr;
    }
} // namespace Quotes
