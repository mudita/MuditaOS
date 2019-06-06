
/*
 * @file ContactsDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "ContactsDB.hpp"

const char *ContactsDB::dbName = "sys/db/contacts.db";


ContactsDB::ContactsDB():
        Database(dbName),
        contacts(this),
        name(this),
        number(this),
        ringtones(this),
        address(this){

    if(contacts.Create() == false) return;
    if(name.Create() == false) return;
    if(number.Create() == false) return;
    if(ringtones.Create() == false) return;
    if(address.Create() == false) return;

    isInitialized = true;
}

ContactsDB::~ContactsDB() {

}