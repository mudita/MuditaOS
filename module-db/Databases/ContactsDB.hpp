
/*
 * @file ContactsDB.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#pragma once

#include "Database/Database.hpp"
#include "../Tables/ContactsTable.hpp"
#include "../Tables/ContactsNameTable.hpp"
#include "../Tables/ContactsNumberTable.hpp"
#include "../Tables/ContactsRingtonesTable.hpp"
#include "../Tables/ContactsAddressTable.hpp"

class ContactsDB : public Database {
public:
    ContactsDB();
    ~ContactsDB();

    static const char* GetDBName(){return dbName;}

    ContactsTable contacts;
    ContactsNameTable name;
    ContactsNumberTable number;
    ContactsRingtonesTable ringtones;
    ContactsAddressTable    address;

private:
    static const char *dbName;
};

