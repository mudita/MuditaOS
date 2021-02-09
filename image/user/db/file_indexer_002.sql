-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

INSERT OR REPLACE INTO file_tab (file_id, path, size, mime_type, mtime, directory, file_type) VALUES
    (1, 'mp3/track1.mp3', 456666, 1, 1603929600, 'mp3',12297),
    (2, 'mp3/track2.mp3', 345354 ,1, 1603929604, 'mp3',12297),
    (3, 'mp3/track3.mp3', 34534 ,1, 1603929604, 'mp3',12297),
    (4, 'mp3/track4.mp3', 345354 ,1, 1603929604, 'mp3',12297);


INSERT OR REPLACE INTO metadata_tab (file_id, property, value) VALUES
    (1,'artist','Sting'),
    (1,'genre','Rock'),
    (1,'album','Album1'),
    (2,'artist','Madonna'),
    (2,'genre','Rock'),
    (2,'album','Album2'),
    (3,'artist','Lady Gaga'),
    (3,'genre','Rock'),
    (3,'album','Album3'),
    (4,'artist','Nick Levis'),
    (4,'genre','Soul'),
    (4,'album','Album4');

INSERT OR REPLACE INTO notifications_tab (id, path, service) VALUES
    (1,'mp3','ServiceDB');
