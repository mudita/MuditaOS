
/*
 * @file SmsDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <module-vfs/vfs.hpp>
#include "SmsDB.hpp"

const char *SmsDB::dbName = USER_FILE("sms.db");

SmsDB::SmsDB() : Database(dbName), sms(this), threads(this)
{

    if (sms.Create() == false)
        return;
    if (threads.Create() == false)
        return;

    isInitialized = true;
}

SmsDB::~SmsDB()
{}