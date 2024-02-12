// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "common/models/AbstractSettingsModel.hpp"

#include <service-db/QuotesMessages.hpp>
#include <ApplicationCommon.hpp>

namespace app
{
    using QuoteCallback = std::function<void(const UTF8 &quote, const UTF8 &author)>;

    class AbstractQuoteModel
    {
      public:
        virtual ~AbstractQuoteModel() noexcept                      = default;
        virtual void sendQuery()                                    = 0;
        virtual bool onQuoteRetreived(db::QueryResult *queryResult) = 0;
        virtual void setCallback(QuoteCallback callback)            = 0;
    };

    class QuoteModel : public AbstractQuoteModel, public app::AsyncCallbackReceiver
    {
      public:
        explicit QuoteModel(app::ApplicationCommon *app);
        void sendQuery() override;
        bool onQuoteRetreived(db::QueryResult *queryResult) override;
        void setCallback(QuoteCallback callback) override;

      private:
        app::ApplicationCommon *app;
        QuoteCallback quoteCallback;
    };
} // namespace app
