// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AsyncTask.hpp>
#include <ApplicationCommon.hpp>
#include <service-db/Settings.hpp>
#include <service-db/QuotesMessages.hpp>
#include "QuotesSettingsSerializer.hpp"

namespace Quotes
{
    using namespace std::chrono;
    inline constexpr auto quotesChangingInterval = duration_cast<seconds>(hours{24}).count();

    class RandomizedQuoteModel
    {
      private:
        app::ApplicationCommon *app = nullptr;
        std::unique_ptr<settings::Settings> settings;
        Database *quotesDB = nullptr;
        void populateList(std::unique_ptr<QuotesList> quotesList, bool forcedUpdate = false);
        void shiftIdList();
        auto isIdExpired() -> bool;
        void randomize(IdList &list);
        std::unique_ptr<QuotesSettingsSerializer> serializer;

      public:
        RandomizedQuoteModel(std::unique_ptr<settings::Settings> settings, Database *quotesDB);
        void updateList(bool forced);
        [[nodiscard]] auto getId() -> int;
    };

} // namespace Quotes
