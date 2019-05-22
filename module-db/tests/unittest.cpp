/*
 * @file unittests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 22.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "vfs.hpp"

#include "catch.hpp"

#include <stdint.h>
#include <algorithm>

#include <iostream>

#include "../database/sqlite3.h"


class vfs vfs;


TEST_CASE( "Create and destroy simple database" ) {

    sqlite3* db = nullptr;

    auto rc = sqlite3_open("test.db", &db);\
    REQUIRE(rc == SQLITE_OK);

    sqlite3_close(db);
}
