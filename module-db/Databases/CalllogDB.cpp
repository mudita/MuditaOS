// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <vfs.hpp>
#include "CalllogDB.hpp"

const char *CalllogDB::dbName = USER_PATH("calllog.db");

CalllogDB::CalllogDB() : Database(dbName), calls(this)
{

    if (calls.create() == false)
        return;
    isInitialized_ = true;
}

CalllogDB::~CalllogDB()
{}
