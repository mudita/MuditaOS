
/*
 * @file ContactsDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <module-vfs/vfs.hpp>
#include "ContactsDB.hpp"

const char *ContactsDB::dbName = USER_PATH("contacts.db");

ContactsDB::ContactsDB() : Database(dbName), contacts(this), name(this), number(this), ringtones(this), address(this)
{

    if (contacts.create() == false)
        return;
    if (name.create() == false)
        return;
    if (number.create() == false)
        return;
    if (ringtones.create() == false)
        return;
    if (address.create() == false)
        return;

    isInitialized = true;
}

ContactsDB::~ContactsDB()
{}
