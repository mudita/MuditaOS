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

#include "../database/Database.hpp"


class vfs vfs;


TEST_CASE( "Create and destroy simple database" ) {

    db database;


    SECTION("Create database")
    {

        Database testDB("test.db");

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

        const char* testdb_queries[] = {media_artist_table, media_album_table,media_songs_table};

        //execute all commands from the array
        for( uint32_t i=0; i<sizeof(testdb_queries)/sizeof(char*); i++)
        {
            REQUIRE(testDB.Execute(testdb_queries[i]) == true);
        }

    }

    SECTION("Add records to database")
    {
        Database testDB("test.db");

        REQUIRE(testDB.Execute("insert or ignore into artists ( name ) VALUES ( '%s');","Mati Patus") == true);
        REQUIRE(testDB.Execute("insert or ignore into artists ( name ) VALUES ( '%s');","Mati Patus2") == true);
        REQUIRE(testDB.Execute("insert or ignore into artists ( name ) VALUES ( '%s');","Mati Patus3") == true);
        REQUIRE(testDB.Execute("insert or ignore into artists ( name ) VALUES ( '%s');","Mati Patus4") == true);
        REQUIRE(testDB.Execute("insert or ignore into artists ( name ) VALUES ( '%s');","Mati Patus5") == true);
        REQUIRE(testDB.Execute("insert or ignore into artists ( name ) VALUES ( '%s');","Mati Patus6") == true);


    }

    SECTION("Query database")
    {
        Database testDB("test.db");

        const char* name = "Mati Patus6";
        auto queryRes = testDB.Query("SELECT * from artists;");

        REQUIRE(queryRes->GetFieldCount() == 2);
        REQUIRE(queryRes->GetRowCount() ==6);

        do{
            std::cout << "Artist ID: " <<  (*queryRes)[0].GetInt32() << "\n";
            std::cout << "Artist name: " <<  (*queryRes)[1].GetString() << "\n";
        }while(queryRes->NextRow());




    }



}
