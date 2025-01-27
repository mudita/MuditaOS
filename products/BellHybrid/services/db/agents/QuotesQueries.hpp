// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Common/Types.hpp>

namespace Quotes::Queries
{
    inline constexpr auto getPredefinedQuotes =
        "SELECT QT.quote_id FROM quote_table as QT, quote_languages as QL WHERE QL.lang_name=" str_
        " and QT.quote_lang=QL.lang_id GROUP BY QT.quote_id;";

    inline constexpr auto readPredefinedQuote =
        "SELECT quote_id, quote, author FROM quote_table WHERE quote_id=" u32_ ";";

    inline constexpr auto getCustomQuotes = "SELECT quote_id FROM custom_quote_table;";

    inline constexpr auto readCustomQuote =
        "SELECT quote_id, quote, author FROM custom_quote_table WHERE quote_id=" u32_ ";";

    inline constexpr auto getCustomQuotesCount = "SELECT COUNT(quote_id) FROM custom_quote_table;";
} // namespace Quotes::Queries
