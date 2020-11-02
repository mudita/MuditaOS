R"dbInitStr(

/* Create File indexer  tables */
BEGIN TRANSACTION;


-- Main file table
CREATE TABLE IF NOT EXISTS file_tab (
    file_id INTEGER NOT NULL,
    path TEXT NOT NULL UNIQUE,
    size INTEGER,
    mime_type INTEGER,
    mtime INTEGER,
    directory TEXT,
    file_type INTEGER,
    PRIMARY KEY (file_id)
    );

-- Table contains information
-- about file metadata.
CREATE TABLE IF NOT EXISTS metadata_tab (
    file_id INTEGER NOT NULL,
    property TEXT NOT NULL,
    value TEXT NOT NULL,
    PRIMARY KEY (file_id, property),
    FOREIGN KEY (file_id) REFERENCES file_tab(file_id)
    );

-- Table contains information who to inform
-- about changes in values.
CREATE TABLE IF NOT EXISTS notifications_tab (
    id INTEGER PRIMARY KEY,
    path TEXT NOT NULL,
    service TEXT,
    CONSTRAINT notification_unique
        UNIQUE(path, service)
);

-- ----------- insert default values ----------------------
INSERT OR REPLACE INTO file_tab (path, size, mime_type, mtime, directory, file_type) VALUES
    ('mp3/track1.mp3', 456666, 1, 1603929600, 'music',12297),
    ('mp3/track2.mp3', 345354 ,1, 1603929604, 'my_songs',12297);


INSERT OR REPLACE INTO metadata_tab (file_id, property, value) VALUES
    (1,'artist','Sting'),
    (1,'genre','Rock'),
    (1,'album','Album1'),
    (2,'artist','Nick Levis'),
    (2,'genre','Soul'),
    (2,'album','Album2');

COMMIT;

)dbInitStr"
