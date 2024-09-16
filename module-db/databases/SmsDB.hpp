// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "module-db/Tables/SMSTable.hpp"
#include "module-db/Tables/ThreadsTable.hpp"
#include "module-db/Tables/SMSTemplateTable.hpp"

class SmsDB : public Database
{
  public:
    SmsDB(const char *name);
    ~SmsDB() = default;

    SMSTable sms;
    ThreadsTable threads;
    SMSTemplateTable templates;
};
