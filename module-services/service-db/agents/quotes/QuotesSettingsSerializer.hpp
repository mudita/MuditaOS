// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <deque>
#include <string>

namespace Quotes
{
    enum class QuoteType
    {
        Predefined,
        Custom
    };

    using QuoteID = std::pair<QuoteType, int>;
    using IdList  = std::deque<QuoteID>;

    class QuotesSettingsSerializer
    {
      public:
        [[nodiscard]] auto serialize(const IdList &list) -> std::string;
        [[nodiscard]] auto deserialize(const std::string &listString) -> IdList;
    };
} // namespace Quotes
