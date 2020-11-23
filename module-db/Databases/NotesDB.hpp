// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "../Tables/NotesTable.hpp"

class NotesDB : public Database
{
  public:
    NotesDB();
    ~NotesDB() = default;

    NotesTable notes;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
