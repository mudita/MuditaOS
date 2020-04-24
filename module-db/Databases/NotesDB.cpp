/*
 * @file NotesDB.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <module-vfs/vfs.hpp>
#include "NotesDB.hpp"

const char *NotesDB::dbName = USER_FILE("notes.db");

NotesDB::NotesDB() : Database(dbName), notes(this)
{

    if (notes.Create() == false)
        return;
    isInitialized = true;
}

NotesDB::~NotesDB()
{}
