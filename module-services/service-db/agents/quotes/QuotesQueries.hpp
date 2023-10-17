// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Common/Types.hpp>

namespace Quotes::Queries
{
    /// To predefined quotes table

    constexpr auto getAllCategories =
        " SELECT CT.category_id, CT.category_name, CT.enabled FROM category_table as CT, quote_languages as QL WHERE "
        "QL.lang_name = " str_ " and CT.lang_id = QL.lang_id;";

    constexpr auto getEnabledPredefinedQuotes = R"sql(
                        SELECT QT.quote_id, QT.quote, QT.author, CT.enabled
                        FROM
                            quote_table as QT,
                            quote_category_map as QCM,
                            category_table as CT,
                            quote_languages as QL
                        WHERE
                            QL.lang_name = '%q'
                            and
                            CT.lang_id = QL.lang_id
                            and
                            CT.enabled = TRUE
                            and 
                            QCM.category_id = CT.category_id
                            and
                            QCM.quote_id = QT.quote_id
                        GROUP BY
                            QCM.quote_id   
                        )sql";

    constexpr auto enableCategory = "UPDATE category_table SET enabled=" u32_ " WHERE category_id=" u32_ ";";

    constexpr auto readPredefinedQuote =
        "SELECT QT.quote_id, QT.quote, QT.author, CT.enabled "
        "FROM quote_table as QT,quote_category_map as QCM,category_table as CT WHERE "
        "QT.quote_id=" u32_ " and QCM.quote_id = QT.quote_id and QCM.category_id = CT.category_id;";

    /// To custom quotes database

    constexpr auto getAllCustomQuotes = R"sql(
                    SELECT quote_id, quote, author, enabled
                    FROM quote_table
                    )sql";

    constexpr auto getEnabledCustomQuotes = R"sql(
                        SELECT quote_id, quote, author, enabled
                        FROM
                            quote_table
                        WHERE
                            enabled = TRUE
                        )sql";

    constexpr auto enableQuote = "UPDATE quote_table SET enabled=" u32_ " WHERE quote_id=" u32_ ";";

    constexpr auto addQuote = "INSERT INTO quote_table (quote, author, enabled) VALUES (" str_c str_c u32_ ");";

    constexpr auto readCustomQuote =
        "SELECT quote_id, quote, author, enabled FROM quote_table WHERE quote_id=" u32_ ";";

    constexpr auto writeQuote =
        "UPDATE quote_table SET quote=" str_c "author=" str_c "enabled=" u32_ " WHERE quote_id=" u32_ ";";

    constexpr auto deleteQuote = "DELETE FROM quote_table WHERE quote_id=" u32_ ";";
} // namespace Quotes::Queries
