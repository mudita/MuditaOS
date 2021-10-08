// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "../Tables/NotesTable.hpp"

class NotesDB : public Database
{
  public:
    NotesDB(const char *name);
    ~NotesDB() = default;

    NotesTable notes;
};
