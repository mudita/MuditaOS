// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NotificationsDB.hpp"

NotificationsDB::NotificationsDB(const char *name) : Database(name), notifications(this)
{}
