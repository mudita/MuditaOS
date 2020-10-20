// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * @file CalllogDB.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <module-vfs/vfs.hpp>
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
