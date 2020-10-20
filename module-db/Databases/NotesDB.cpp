// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-vfs/vfs.hpp>
#include "NotesDB.hpp"

const char *NotesDB::dbName = USER_PATH("notes.db");

NotesDB::NotesDB() : Database(dbName), notes(this)
{

    if (notes.create() == false)
        return;
    isInitialized_ = true;
}

NotesDB::~NotesDB()
{}
