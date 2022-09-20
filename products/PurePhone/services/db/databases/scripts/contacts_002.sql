-- Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

INSERT OR REPLACE INTO contact_group_protected
    (_id, group_id)
VALUES (1, 1),
       (2, 2),
       (3, 3),
       (4, 4);

INSERT OR REPLACE INTO contact_groups
    (_id, name)
VALUES (1, 'Favourites'),
       (2, 'ICE'),
       (3, 'Blocked'),
       (4, 'Temporary');
