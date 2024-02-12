// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Types.hpp>

namespace Quotes::Queries
{
    inline constexpr auto getQuotes =
        "SELECT QT.quote_id FROM quote_table as QT, quote_languages as QL WHERE QL.lang_name=" str_
        " and QT.quote_lang=QL.lang_id GROUP BY QT.quote_id;";

    inline constexpr auto readQuote = "SELECT quote_id, quote, author FROM quote_table WHERE quote_id=" u32_ ";";
} // namespace Quotes::Queries
