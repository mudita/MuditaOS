// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "SmsDB.hpp"

SmsDB::SmsDB(const char *name) : Database(name), sms(this), threads(this), templates(this)
{}
