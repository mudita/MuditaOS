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
    ContactsDB();
    ~ContactsDB();

    static const char *GetDBName()
    {
        return dbName;
    }

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
    static const char *dbName;
    static uint32_t favouritesId;
    static uint32_t iceId;
    static uint32_t blockedId;
    static uint32_t temporaryId;
};
