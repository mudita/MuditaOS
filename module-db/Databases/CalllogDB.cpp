/*
 * @file CalllogDB.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CalllogDB.hpp"

const char *CalllogDB::dbName = "sys/calllog.db";

CalllogDB::CalllogDB() : Database(dbName), calls(this)
{

    if (calls.Create() == false)
        return;
    isInitialized = true;
}

CalllogDB::~CalllogDB()
{}
