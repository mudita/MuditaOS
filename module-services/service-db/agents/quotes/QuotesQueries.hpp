// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace Quotes::Queries
{
    constexpr auto getAllCategories = R"sql(
                        SELECT category_id, category_name, enabled
                        FROM category_table;
                        )sql";

    constexpr auto getAllQuotes = R"sql(
                        SELECT quote_id, lang_id, quote, author, enabled
                        FROM quote_table;
                        )sql";
} // namespace Quotes::Queries
