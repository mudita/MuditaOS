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
#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <iostream>

#include "../database/db.hpp"


class vfs vfs;


TEST_CASE( "Create and destroy simple database" ) {

    db::Init();


    SECTION("Create database")
    {
        sqlite3* db = nullptr;

        auto rc = sqlite3_open("test.db", &db);
            REQUIRE(rc == SQLITE_OK);


        const char* media_album_table =
                "CREATE TABLE IF NOT EXISTS albums("
                "_id	        		INTEGER PRIMARY KEY,"
                "artist_id	            INTEGER,"
                "name	        		TEXT UNIQUE,"
                "FOREIGN KEY(artist_id) REFERENCES artists(_id)"
                ");";

        const char* media_artist_table =
                "CREATE TABLE IF NOT EXISTS artists("
                "_id	                INTEGER PRIMARY KEY,"
                "name   	            TEXT UNIQUE"
                ");";

        const char* media_songs_table =
                "CREATE TABLE IF NOT EXISTS tracks("
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

        const char* _media_queries[] = {media_artist_table, media_album_table,media_songs_table};

        sqlite3_stmt *stmt = NULL;  //binary SQL statement object produced by sqlite3_prepare_v2


        //execute all commands from the array
        for( uint32_t i=0; i<sizeof(_media_queries)/sizeof(char*); i++)
        {
            //create byte code for request
            REQUIRE(sqlite3_prepare_v2(db, _media_queries[i], -1, &stmt, NULL) == SQLITE_OK);
            rc = sqlite3_step(stmt);
            REQUIRE(stmt != nullptr);
            REQUIRE(rc == SQLITE_DONE);

            sqlite3_finalize(stmt);

        }

        sqlite3_close(db);
    }

    SECTION("Add records to database")
    {
        sqlite3* db = nullptr;

        auto rc = sqlite3_open("test.db", &db);
        REQUIRE(rc == SQLITE_OK);


        //create new artist record
        char* sql_cmd = sqlite3_mprintf( "insert or ignore into artists ( name ) VALUES ( '%s');",
                                         "Mati Patus");

        REQUIRE(sql_cmd != nullptr);

        int result = sqlite3_exec(db, sql_cmd, NULL, NULL, NULL );
        REQUIRE(result == SQLITE_OK);
        sqlite3_free(sql_cmd);

        sqlite3_close(db);
    }



}
