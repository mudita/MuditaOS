// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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

    enum class ListUpdateMode
    {
        Normal,
        Forced
    };

    class RandomizedQuoteModel
    {
      private:
        static constexpr auto zeroOffset             = 0;
        static constexpr auto maxLimit               = std::numeric_limits<unsigned>::max();
        static constexpr auto quotesChangingInterval = duration_cast<seconds>(24h).count();

        app::ApplicationCommon *app = nullptr;
        std::unique_ptr<settings::Settings> settings;
        Database *predefinedQuotesDB = nullptr;
        Database *customQuotesDB     = nullptr;
        void populateList(std::unique_ptr<QuotesList> predefinedQuotesList,
                          std::unique_ptr<QuotesList> customQuotesList,
                          ListUpdateMode updateMode = ListUpdateMode::Normal);
        void shiftIdList();
        auto isIdExpired() -> bool;
        void randomize(IdList &list);
        std::unique_ptr<QuotesSettingsSerializer> serializer;
        std::unique_ptr<std::mt19937> rngEngine;

      public:
        RandomizedQuoteModel(std::unique_ptr<settings::Settings> settings,
                             Database *predefinedQuotesDB,
                             Database *customQuotesDB);
        void updateList(ListUpdateMode updateMode);
        [[nodiscard]] auto getId() -> QuoteID;
    };

} // namespace Quotes
