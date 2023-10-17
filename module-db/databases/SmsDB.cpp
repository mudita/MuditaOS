// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SmsDB.hpp"

SmsDB::SmsDB(const char *name) : Database(name), sms(this), threads(this), templates(this)
{}
