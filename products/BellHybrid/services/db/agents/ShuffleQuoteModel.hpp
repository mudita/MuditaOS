// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>
#include <service-db/Settings.hpp>
#include <service-db/QuotesMessages.hpp>
#include <service-db/agents/quotes/QuotesSettingsSerializer.hpp>

namespace Quotes
{
    enum class ListUpdateMode
    {
        Normal,
        Forced
    };

    class ShuffleQuoteModel
    {
      private:
        app::ApplicationCommon *app{nullptr};
        std::unique_ptr<settings::Settings> settings;
        Database *quotesDB{nullptr};
        std::unique_ptr<QuotesSettingsSerializer> serializer;
        std::unique_ptr<std::mt19937> rngEngine;

        auto populateList(std::unique_ptr<IdsList> idsList, ListUpdateMode updateMode) -> void;
        auto shiftIdList() -> void;
        auto isQuoteExpired() -> bool;

      public:
        ShuffleQuoteModel(std::unique_ptr<settings::Settings> settings, Database *quotesDB);
        auto updateList(ListUpdateMode updateMode) -> void;
        [[nodiscard]] auto getId() -> int;
    };

} // namespace Quotes
