-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CREATE TABLE IF NOT EXISTS contacts
(
    _id        INTEGER PRIMARY KEY,
    name_id    INTEGER,
    numbers_id TEXT NOT NULL,
    ring_id    INTEGER,
    address_id INTEGER,
    speeddial  TEXT NOT NULL,
    FOREIGN KEY (name_id) REFERENCES contact_name (_id),
    FOREIGN KEY (ring_id) REFERENCES contact_ringtones (_id)
);

CREATE TABLE IF NOT EXISTS contact_address
(
    _id        INTEGER PRIMARY KEY,
    contact_id INTEGER,
    address    TEXT NOT NULL,
    note       TEXT NOT NULL,
    mail       TEXT NOT NULL,
    FOREIGN KEY (contact_id) REFERENCES contacts (_id)
);

CREATE TABLE IF NOT EXISTS contact_groups
(
    _id  INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS contact_match_groups
(
    _id        INTEGER PRIMARY KEY,
    group_id   INTEGER,
    contact_id INTEGER,
    FOREIGN KEY (group_id) REFERENCES contact_groups (_id)
        ON DELETE CASCADE,
    FOREIGN KEY (contact_id) REFERENCES contacts (_id)
        ON DELETE CASCADE,
    CONSTRAINT unique_group_contact
        UNIQUE (group_id, contact_id)
);

CREATE TABLE IF NOT EXISTS contact_group_protected
(
    _id      INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    group_id INTEGER NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS contact_name
(
    _id              INTEGER PRIMARY KEY,
    contact_id       INTEGER,
    name_primary     TEXT NOT NULL,
    name_alternative TEXT NOT NULL,
    FOREIGN KEY (contact_id) REFERENCES contacts (_id)
);

CREATE TABLE IF NOT EXISTS contact_number
(
    _id         INTEGER PRIMARY KEY,
    contact_id  INTEGER,
    number_user TEXT NOT NULL,
    number_e164 TEXT NOT NULL,
    type        INTEGER,
    FOREIGN KEY (contact_id) REFERENCES contacts (_id)
);

CREATE TABLE IF NOT EXISTS contact_ringtones
(
    _id        INTEGER PRIMARY KEY,
    contact_id INTEGER,
    asset_path TEXT NOT NULL,
    FOREIGN KEY (contact_id) REFERENCES contacts (_id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS contact_match_group_index_on_group
    ON contact_match_groups (group_id);
CREATE INDEX IF NOT EXISTS contact_match_group_index_on_contact
    ON contact_match_groups (contact_id);

