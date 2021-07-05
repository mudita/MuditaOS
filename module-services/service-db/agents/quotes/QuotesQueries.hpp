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

    constexpr auto getQuotesByCategoryId = R"sql(
                        SELECT QT.quote_id, QT.lang_id, QT.quote, QT.author, QT.enabled
                        FROM 
                            quote_table as QT,
                            quote_category_map as QCM,
                            category_table as CT
                        WHERE
                            QCM.quote_id = QT.quote_id
                            and 
                            QCM.category_id = CT.category_id
                            and
                            QCM.category_id = '%lu'
                            and
                            CT.enabled = TRUE
                        )sql";

    constexpr auto getQuotesFromCustomCategory = R"sql(
                        SELECT QT.quote_id, QT.lang_id, QT.quote, QT.author, QT.enabled
                        FROM
                            quote_table as QT,
                            quote_category_map as QCM,
                            category_table as CT
                        WHERE
                            QCM.quote_id = QT.quote_id
                            and 
                            QCM.category_id = CT.category_id
                            and
                            CT.category_name = 'Custom'
                            and
                            CT.enabled = TRUE
                        )sql";

    constexpr auto getCustomCategoryId = R"sql(
                        SELECT category_id, category_name, enabled
                        FROM
                            category_table
                        WHERE
                            category_name = 'Custom'
                        )sql";

    constexpr auto enableCategory = R"sql(
                        UPDATE category_table SET enabled = '%d'
                        WHERE category_id = '%lu';
                        )sql";

    constexpr auto enableQuote = R"sql(
                        UPDATE quote_table SET enabled = '%d'
                        WHERE quote_id = '%lu';
                        )sql";

    constexpr auto getEnabledQuotes = R"sql(
                        SELECT QT.quote_id, QT.lang_id, QT.quote, QT.author, QT.enabled
                        FROM
                            quote_table as QT,
                            quote_category_map as QCM,
                            category_table as CT
                        WHERE
                            QCM.quote_id = QT.quote_id
                            and
                            QCM.category_id = CT.category_id
                            and
                            QT.enabled = TRUE
                            and
                            CT.enabled = TRUE
                        )sql";

    constexpr auto addQuoteToQuoteTable = R"sql(
                        INSERT INTO quote_table (lang_id, quote, author, enabled)
                        VALUES ('%lu', '%q' , '%q', '%d');
                        )sql";

    constexpr auto addQuoteToQuoteCategoryMapTable = R"sql(
                        INSERT INTO quote_category_map (category_id, quote_id)
                        VALUES ('%lu', '%lu');
                        )sql";

    constexpr auto readQuote = R"sql(
                        SELECT quote_id, lang_id, quote, author, enabled
                        FROM quote_table
                        WHERE quote_id = '%lu';
                        )sql";

    constexpr auto writeQuote = R"sql(
                        UPDATE quote_table
                        SET lang_id = '%lu', quote = '%q', author = '%q', enabled = '%d'
                        WHERE quote_id = '%lu';
                        )sql";

    constexpr auto deleteQuoteFromQuoteCategoryMapTable = R"sql(
                        DELETE FROM quote_category_map
                        WHERE quote_id = '%lu';
                        )sql";

    constexpr auto deleteQuoteFromQuoteTable = R"sql(
                        DELETE FROM quote_table
                        WHERE quote_id = '%lu';
                        )sql";
} // namespace Quotes::Queries
