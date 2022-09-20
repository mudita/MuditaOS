-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS quote_languages (
    lang_id INTEGER NOT NULL,
    lang_name TEXT NOT NULL,
    PRIMARY KEY (lang_id)
    );

CREATE TABLE IF NOT EXISTS category_table (
    category_id INTEGER NOT NULL,
    lang_id INTEGER NOT NULL,
    category_name TEXT NOT NULL UNIQUE,
    enabled BOOLEAN NOT NULL DEFAULT TRUE,
    PRIMARY KEY (category_id),
    FOREIGN KEY (lang_id) REFERENCES quote_languages(lang_id)
    );

CREATE TABLE IF NOT EXISTS quote_table (
    quote_id INTEGER NOT NULL,
    quote TEXT NOT NULL,
    author TEXT,
    PRIMARY KEY (quote_id)
    );

CREATE TABLE IF NOT EXISTS quote_category_map (
    category_id INTEGER NOT NULL,
    quote_id INTEGER NOT NULL,
    FOREIGN KEY (category_id) REFERENCES category_table(category_id),
    FOREIGN KEY (quote_id) REFERENCES quote_table(quote_id),
    CONSTRAINT quotes_unique UNIQUE(category_id, quote_id)
    );
