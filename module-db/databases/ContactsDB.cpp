// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ContactsDB.hpp"

uint32_t ContactsDB::favouritesId = 0;
uint32_t ContactsDB::iceId        = 0;
uint32_t ContactsDB::blockedId    = 0;
uint32_t ContactsDB::temporaryId  = 0;

ContactsDB::ContactsDB(const char *name)
    : Database(name), contacts(this), name(this), number(this), ringtones(this), address(this), groups(this)
{

    if (favouritesId == 0) {
        favouritesId = groups.favouritesId();
    }
    if (iceId == 0) {
        iceId = groups.iceId();
    }
    if (blockedId == 0) {
        blockedId = groups.blockedId();
    }
    if (temporaryId == 0) {
        temporaryId = groups.temporaryId();
    }
}
