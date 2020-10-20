// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "../Tables/SMSTable.hpp"
#include "../Tables/ThreadsTable.hpp"
#include "../Tables/SMSTemplateTable.hpp"

class SmsDB : public Database
{
  public:
    SmsDB();
    ~SmsDB();

    SMSTable sms;
    ThreadsTable threads;
    SMSTemplateTable templates;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
