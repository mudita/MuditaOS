// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "CalllogDB.hpp"

CalllogDB::CalllogDB(const char *name) : Database(name), calls(this)
{}
