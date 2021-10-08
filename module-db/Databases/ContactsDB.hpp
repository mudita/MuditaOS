// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Database/Database.hpp>
#include <Tables/ContactsTable.hpp>
#include <Tables/ContactsNameTable.hpp>
#include <Tables/ContactsNumberTable.hpp>
#include <Tables/ContactsRingtonesTable.hpp>
#include <Tables/ContactsAddressTable.hpp>
#include <Tables/ContactsGroups.hpp>

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
