// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/QuotesMessages.hpp>
#include <ApplicationCommon.hpp>
#include <module-services/service-db/agents/quotes/RandomizedQuoteModel.hpp>

namespace gui
{
    class QuotesPresenter : public app::AsyncCallbackReceiver
    {
      public:
        QuotesPresenter(app::ApplicationCommon *app);
        void setQuoteDisplayCallback(std::function<void(std::string, std::string)> callback);
        void requestQuote();

      private:
        bool onQuoteRetreived(db::QueryResult *queryResult);
        app::ApplicationCommon *application;
        std::function<void(std::string, std::string)> quoteDisplayCallback;
    };

} // namespace gui
