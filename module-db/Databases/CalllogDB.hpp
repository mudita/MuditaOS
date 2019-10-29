/*
 * @file CalllogDB.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log DB
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Database/Database.hpp"
#include "../Tables/CalllogTable.hpp"

class CalllogDB : public Database {
public:
	CalllogDB();
    virtual ~CalllogDB();

    CalllogTable calls;

    static const char* GetDBName(){return dbName;}

private:

    static const char *dbName;
};
