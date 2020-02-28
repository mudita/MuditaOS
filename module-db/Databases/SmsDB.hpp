
/*
 * @file SmsDB.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Database/Database.hpp"
#include "../Tables/SMSTable.hpp"
#include "../Tables/ThreadsTable.hpp"

class SmsDB : public Database {
public:
    SmsDB();
    ~SmsDB();

    SMSTable sms;
    ThreadsTable threads;

    static const char* GetDBName(){return dbName;}

private:

    static const char *dbName;
};

