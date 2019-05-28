
/*
 * @file ContactsDB.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTSDB_HPP
#define PUREPHONE_CONTACTSDB_HPP

#include "Database/Database.hpp"
#include "../Tables/ContactsTable.hpp"

class ContactsDB : public Database {
public:
    ContactsDB();
    ~ContactsDB();

    static const char* GetDBName(){return dbName;}

    ContactsTable contacts;

private:
    bool isInitialized;
    static const char *dbName;
};


#endif //PUREPHONE_CONTACTSDB_HPP
