// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <ApplicationCommon.hpp>
#include <service-db/Settings.hpp>
#include <service-db/QuotesMessages.hpp>
#include <service-db/agents/quotes/QuotesSettingsSerializer.hpp>

namespace Quotes
{
    inline constexpr auto customGroup     = "Custom";
    inline constexpr auto predefinedGroup = "Predefined";
    inline constexpr auto atMidnight      = "AtMidnight";

    enum class ListUpdateMode
    {
        Normal,
        Forced
    };

    class ShuffleQuoteModel
    {
      public:
        ShuffleQuoteModel(settings::Settings &settings, Database *quotesDB);
        auto updateList(ListUpdateMode updateMode) -> void;
        [[nodiscard]] auto getId() -> int;

      private:
        app::ApplicationCommon *app{nullptr};
        settings::Settings &settings;
        Database *quotesDB{nullptr};
        std::unique_ptr<QuotesSettingsSerializer> serializer;
        std::unique_ptr<std::mt19937> rngEngine;

        auto populateList(std::unique_ptr<IdsList> idsList, ListUpdateMode updateMode) -> void;
        auto shiftIdList() -> void;
        auto isQuoteExpired() -> bool;
    };

} // namespace Quotes
