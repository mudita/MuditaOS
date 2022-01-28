// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace Quotes::Queries
{
    /// To predefined quotes table

    constexpr auto getAllCategories = R"sql(
                        SELECT CT.category_id, CT.category_name, CT.enabled
                        FROM 
                            category_table as CT,
                            quote_languages as QL
                        WHERE
                            QL.lang_name = '%q'
                            and
                            CT.lang_id = QL.lang_id 
                        )sql";

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

    constexpr auto enableCategory = R"sql(
                        UPDATE category_table SET enabled = '%d'
                        WHERE category_id = '%lu';
                        )sql";

    constexpr auto readPredefinedQuote = R"sql(
                        SELECT QT.quote_id, QT.quote, QT.author, CT.enabled
                        FROM
                            quote_table as QT,
                            quote_category_map as QCM,
                            category_table as CT
                        WHERE
                            QT.quote_id = '%lu'
                            and
                            QCM.quote_id = QT.quote_id
                            and 
                            QCM.category_id = CT.category_id
                        )sql";

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

    constexpr auto enableQuote = R"sql(
                        UPDATE quote_table SET enabled = '%d'
                        WHERE quote_id = '%lu';
                        )sql";

    constexpr auto addQuote = R"sql(
                        INSERT INTO quote_table (quote, author, enabled)
                        VALUES ('%q' , '%q', '%d');
                        )sql";

    constexpr auto readCustomQuote = R"sql(
                        SELECT quote_id, quote, author, enabled
                        FROM quote_table
                        WHERE quote_id = '%lu';
                        )sql";

    constexpr auto writeQuote = R"sql(
                        UPDATE quote_table
                        SET quote = '%q', author = '%q', enabled = '%d'
                        WHERE quote_id = '%lu';
                        )sql";

    constexpr auto deleteQuote = R"sql(
                        DELETE FROM quote_table
                        WHERE quote_id = '%lu';
                        )sql";
} // namespace Quotes::Queries
