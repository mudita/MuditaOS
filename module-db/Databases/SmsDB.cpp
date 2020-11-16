// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <vfs.hpp>
#include "SmsDB.hpp"

const char *SmsDB::dbName = USER_PATH("sms.db");

SmsDB::SmsDB() : Database(dbName), sms(this), threads(this), templates(this)
{
    if (sms.create() == false)
        return;
    if (threads.create() == false)
        return;
    if (templates.create() == false)
        return;

    isInitialized_ = true;
}

SmsDB::~SmsDB()
{}
