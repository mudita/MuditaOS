// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include "Database/Database.hpp"
#include "Database/DatabaseInitializer.hpp"

#include "Tables/SMSTable.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

#include <cstdint>
#include <cstdio>
#include <cstring>

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
        std::string backupPathDB = "testbackup.db";
        std::remove(backupPathDB.c_str());
        Database testDB("test.db");
        REQUIRE(testDB.storeIntoFile(backupPathDB) == true);
        std::ifstream f(backupPathDB);
        REQUIRE(f.good() == true);
    }

    Database::deinitialize();
}

class ScopedDir
{
  public:
    ScopedDir(std::string p) : path(p)
    {
        if (!(std::filesystem::exists(path.c_str()))) {
            REQUIRE(std::filesystem::create_directory(path.c_str()));
        }
    }

    ~ScopedDir()
    {
        if (std::filesystem::exists(path.c_str())) {
            REQUIRE(std::filesystem::remove_all(path.c_str()) > 0);
        }
    }

    auto operator()(std::string file = "") -> std::filesystem::path
    {
        return path / file;
    }

  private:
    std::filesystem::path path;
};

TEST_CASE("Database initialization scripts")
{
    Database::initialize();

    const char *script_create = "CREATE TABLE IF NOT EXISTS tracks("
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

    const char *script_insert = "insert or ignore into artists(name) VALUES('%q');";

    const char *script_comment = "--insert or ignore into artists(name) VALUES('%q');\n"
                                 "insert or ignore into artists(name) VALUES('%q');\n"
                                 "--insert or ignore into artists(name) VALUES('%q');\n"
                                 "insert or ignore into artists(name) VALUES('%q');\n";

    const char *script_invalid = "insert artists(name) VALUES('%q');";

    SECTION("list files")
    {
        ScopedDir dir("scripts");

        auto file = std::fopen(dir("test_1.sql").c_str(), "w");
        std::fclose(file);

        file = std::fopen(dir("test_021.sql").c_str(), "w");
        std::fclose(file);

        file = std::fopen(dir("test_011.sql").c_str(), "w");
        std::fclose(file);

        file = std::fopen(dir("test_003.sql").c_str(), "w");
        std::fclose(file);

        file = std::fopen(dir("noprefix_003.sql").c_str(), "w");
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        auto files = initializer.listFiles("scripts", "test", "sql");

        REQUIRE(files.size() == 4);
        REQUIRE(files[0] == (std::filesystem::path{"scripts"} / "test_1.sql"));
        REQUIRE(files[1] == (std::filesystem::path{"scripts"} / "test_003.sql"));
        REQUIRE(files[2] == (std::filesystem::path{"scripts"} / "test_011.sql"));
        REQUIRE(files[3] == (std::filesystem::path{"scripts"} / "test_021.sql"));
    }

    SECTION("read script files")
    {
        ScopedDir dir("scripts");

        auto file = std::fopen(dir("test_1.sql").c_str(), "w");

        std::fprintf(file, "%s\n", script_create);
        std::fprintf(file, "%s\n", script_insert);

        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        auto commands = initializer.readCommands(dir("test_1.sql"));

        REQUIRE(commands.size() == 2);
    }

    SECTION("read empty script files")
    {
        ScopedDir dir("scripts");

        auto file = std::fopen(dir("test_1.sql").c_str(), "w");
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        auto commands = initializer.readCommands(dir("test_1.sql"));

        REQUIRE(commands.size() == 0);
    }

    SECTION("read script file with comment")
    {
        ScopedDir dir("scripts");

        auto file = std::fopen(dir("test_1.sql").c_str(), "w");
        std::fprintf(file, "%s\n", script_comment);
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        auto commands = initializer.readCommands(dir("test_1.sql"));

        REQUIRE(commands.size() == 2);
    }

    SECTION("execute valid script")
    {
        ScopedDir dir("scripts");

        auto file = std::fopen(dir("test_1.sql").c_str(), "w");
        std::fprintf(file, "%s\n", script_create);
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        bool r = initializer.run(dir());

        REQUIRE(r == true);
    }

    SECTION("execute invalid script")
    {
        ScopedDir dir("scripts");

        auto file = std::fopen(dir("test_1.sql").c_str(), "w");
        std::fprintf(file, "%s\n", script_invalid);
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        bool result = initializer.run(dir());

        REQUIRE(result == false);
    }

    REQUIRE(Database::deinitialize() == true);
}
