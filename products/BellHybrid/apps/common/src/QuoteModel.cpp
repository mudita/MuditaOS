// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "models/QuoteModel.hpp"

namespace app
{
    QuoteModel::QuoteModel(app::ApplicationCommon *app) : app::AsyncCallbackReceiver(app), app(app)
    {
        sendQuery();
    }

    void QuoteModel::setCallback(QuoteCallback callback)
    {
        quoteCallback = callback;
    }

    void QuoteModel::sendQuery()
    {
        auto query = std::make_unique<Quotes::Messages::ReadRandomizedQuoteRequest>();
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
        task->setCallback([this](auto response) { return onQuoteRetreived(response); });
        task->execute(app, this);
    }

    bool QuoteModel::onQuoteRetreived(db::QueryResult *queryResult)
    {
        auto response = dynamic_cast<Quotes::Messages::ReadRandomizedQuoteResponse *>(queryResult);
        if (response && quoteCallback) {
            quoteCallback(response->quote, response->author);
        }
        return true;
    }
} // namespace app
