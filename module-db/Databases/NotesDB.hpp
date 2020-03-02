/*
 * @file NotesDB.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Database/Database.hpp"
#include "../Tables/NotesTable.hpp"

class NotesDB : public Database {
public:
	NotesDB();
    virtual ~NotesDB();

    NotesTable notes;

    static const char* GetDBName(){return dbName;}

private:

    static const char *dbName;
};

