// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

const char *NotesDB::dbName = USER_PATH("notes.db");

NotesDB::NotesDB() : Database(dbName), notes(this)
{

    if (notes.create() == false)
        return;
    isInitialized_ = true;
}

NotesDB::~NotesDB()
{}
