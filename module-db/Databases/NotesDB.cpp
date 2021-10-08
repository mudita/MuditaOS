// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesDB.hpp"

NotesDB::NotesDB(const char *name) : Database(name), notes(this)
{}
