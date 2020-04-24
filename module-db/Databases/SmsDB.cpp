
#include "SmsDB.hpp"

const char *SmsDB::dbName = "sys/sms.db";

SmsDB::SmsDB() : Database(dbName), sms(this), threads(this), templates(this)
{
    if (sms.Create() == false)
        return;
    if (threads.Create() == false)
        return;
    if (templates.Create() == false)
        return;

    isInitialized = true;
}

SmsDB::~SmsDB()
{}