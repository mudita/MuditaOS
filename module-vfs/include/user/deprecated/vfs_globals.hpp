// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#define PATH_SYS      "/sys"
#define PATH_USER     "user"
#define PATH_CURRENT  "current"
#define PATH_PREVIOUS "previous"
#define PATH_UPDATES  "updates"
#define PATH_TMP      "tmp"
#define PATH_BACKUP   "backup"
#define PATH_FACTORY  "factory"

// this just concatenates two strings and creates a /user/ subdirectory filename
#define USER_PATH(file) PATH_SYS "/" PATH_USER "/" file
