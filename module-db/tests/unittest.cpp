/*
 * @file unittests.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 22.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include "vfs.hpp"

#include <catch2/catch.hpp>

#include "Database/Database.hpp"

#include "Tables/SMSTable.hpp"

#include <algorithm>
#include <iostream>

#include <cstdint>
#include <cstdio>
#include <cstring>

class vfs vfs;

TEST_CASE("Create and destroy simple database")
{

    Database::initialize();

    SECTION("Create database")
    {

        Database testDB("test.db");

        const char *media_album_table = "CREATE TABLE IF NOT EXISTS albums("
                                        "_id	        		INTEGER PRIMARY KEY,"
                                        "artist_id	            INTEGER,"
                                        "name	        		TEXT UNIQUE,"
                                        "FOREIGN KEY(artist_id) REFERENCES artists(_id)"
                                        ");";

        const char *media_artist_table = "CREATE TABLE IF NOT EXISTS artists("
                                         "_id	                INTEGER PRIMARY KEY,"
                                         "name   	            TEXT UNIQUE"
                                         ");";

        const char *media_songs_table = "CREATE TABLE IF NOT EXISTS tracks("
                                        "_id	                INTEGER PRIMARY KEY,"
                                        "filename	            TEXT UNIQUE,"
                                        "name	            	TEXT,"
                                        "duration	            INTEGER,"
                                        "artist_id	            INTEGER,"
                                        "album_id	            INTEGER,"
                                        "cover  	            INTEGER,"
                                        "FOREIGN KEY(artist_id) REFERENCES artists(_id),"
                                        "FOREIGN KEY(album_id) 	REFERENCES albums(_id)"
                                        ");";

        const char *testdb_queries[] = {media_artist_table, media_album_table, media_songs_table};

        // execute all commands from the array
        for (uint32_t i = 0; i < sizeof(testdb_queries) / sizeof(char *); i++) {
            REQUIRE(testDB.execute(testdb_queries[i]) == true);
        }
    }

    SECTION("Add records to database")
    {
        Database testDB("test.db");

        REQUIRE(testDB.execute("insert or ignore into artists ( name ) VALUES ( '%q');", "Mati Patus") == true);
        REQUIRE(testDB.execute("insert or ignore into artists ( name ) VALUES ( '%q');", "Mati Patus2") == true);
        REQUIRE(testDB.execute("insert or ignore into artists ( name ) VALUES ( '%q');", "Mati Patus3") == true);
        REQUIRE(testDB.execute("insert or ignore into artists ( name ) VALUES ( '%q');", "Mati Patus4") == true);
        REQUIRE(testDB.execute("insert or ignore into artists ( name ) VALUES ( '%q');", "Mati Patus5") == true);
        REQUIRE(testDB.execute("insert or ignore into artists ( name ) VALUES ( '%q');", "Mati Patus6") == true);
    }

    SECTION("query database")
    {
        Database testDB("test.db");

        auto queryRes = testDB.query("SELECT * from artists;");

        REQUIRE(queryRes->getFieldCount() == 2);
        REQUIRE(queryRes->getRowCount() == 6);
    }

    SECTION("Store database into backup file")
    {
        std::string backupPathDB = USER_PATH("testbackup.db");
        vfs.remove(backupPathDB.c_str());
        Database testDB("test.db");
        REQUIRE(testDB.storeIntoFile(backupPathDB) == true);
        REQUIRE(vfs.fileExists(backupPathDB.c_str()) == true);
    }

    Database::deinitialize();
}
