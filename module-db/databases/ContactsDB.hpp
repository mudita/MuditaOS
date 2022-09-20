// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-db/Database/Database.hpp"
#include "module-db/Tables/ContactsTable.hpp"
#include "module-db/Tables/ContactsNameTable.hpp"
#include "module-db/Tables/ContactsNumberTable.hpp"
#include "module-db/Tables/ContactsRingtonesTable.hpp"
#include "module-db/Tables/ContactsAddressTable.hpp"
#include "module-db/Tables/ContactsGroups.hpp"

class ContactsDB : public Database
{
  public:
    ContactsDB(const char *name);
    ~ContactsDB() = default;

    ContactsTable contacts;
    ContactsNameTable name;
    ContactsNumberTable number;
    ContactsRingtonesTable ringtones;
    ContactsAddressTable address;
    ContactsGroupsTable groups;

    static uint32_t favouritesGroupId()
    {
        return favouritesId;
    }
    static uint32_t iceGroupId()
    {
        return iceId;
    }
    static uint32_t blockedGroupId()
    {
        return blockedId;
    }
    static uint32_t temporaryGroupId()
    {
        return temporaryId;
    }

  private:
    static uint32_t favouritesId;
    static uint32_t iceId;
    static uint32_t blockedId;
    static uint32_t temporaryId;
};
