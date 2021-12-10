// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesPresenter.hpp"

namespace gui
{
    QuotesPresenter::QuotesPresenter(app::ApplicationCommon *app) : AsyncCallbackReceiver(app), application(app)
    {}

    void QuotesPresenter::setQuoteDisplayCallback(std::function<void(std::string, std::string)> callback)
    {
        quoteDisplayCallback = callback;
    }

    void QuotesPresenter::requestQuote()
    {
        // TODO: get quote: only enabled, from currently selected language
        constexpr auto quoteID = 1;
        auto query             = std::make_unique<Quotes::Messages::ReadQuoteRequest>(quoteID);
        auto task              = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
        task->setCallback([this](auto response) { return onQuoteRetreived(response); });
        task->execute(application, this);
    }

    bool QuotesPresenter::onQuoteRetreived(db::QueryResult *queryResult)
    {
        auto msgResponse = dynamic_cast<Quotes::Messages::ReadQuoteResponse *>(queryResult);
        assert(msgResponse != nullptr);

        if (quoteDisplayCallback) {
            quoteDisplayCallback(msgResponse->quote, msgResponse->author);
        }

        return true;
    }

} // namespace gui
