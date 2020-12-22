// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <vfs.hpp>

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

class vfs vfs;

TEST_CASE("Create and destroy simple database")
{
    vfs.Init();

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
        REQUIRE(std::filesystem::create_directory(path.c_str()));
    }

    ~ScopedDir()
    {
        REQUIRE(std::filesystem::remove(path.c_str()));
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
    vfs.Init();
    Database::initialize();

    const std::string script_create = "CREATE TABLE IF NOT EXISTS tracks("
                                      "_id	                INTEGER PRIMARY KEY,"
                                      "filename	            TEXT UNIQUE,"
                                      "name	            	TEXT,"
                                      "duration	            INTEGER,"
                                      "artist_id	            INTEGER,"
                                      "album_id	            INTEGER,"
                                      "cover  	            INTEGER,"
                                      "FOREIGN KEY(artist_id) REFERENCES artists(_id),"
                                      "FOREIGN KEY(album_id) 	REFERENCES albums(_id)"
                                      ");\n";

    const std::string script_insert = "insert or ignore into artists(name) VALUES('%q');\n";

    const std::string script_comment = "--insert or ignore into artists(name) VALUES('%q');\n"
                                       "insert or ignore into artists(name) VALUES('%q');\n"
                                       "--insert or ignore into artists(name) VALUES('%q');\n"
                                       "insert or ignore into artists(name) VALUES('%q');\n";

    const std::string script_invalid = "insert artists(name) VALUES('%q');\n";

    SECTION("list files")
    {
        ScopedDir dir(USER_PATH("test"));
        LOG_INFO("PATH_SYS: %s", PATH_SYS);
        LOG_INFO("PATH_USER: %s", PATH_USER);

        auto file = std::fopen(dir("test_1.sql").c_str(), "w");
        LOG_INFO("test file: %s", dir("test_1.sql").c_str());
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
        auto files = initializer.listFiles(dir(), "test", "sql");

        REQUIRE(files.size() == 4);
        REQUIRE(files[0] == dir("test_1.sql"));
        REQUIRE(files[1] == dir("test_003.sql"));
        REQUIRE(files[2] == dir("test_011.sql"));
        REQUIRE(files[3] == dir("test_021.sql"));
    }

    SECTION("read script files")
    {
        ScopedDir dir(USER_PATH("scripts"));
        std::string test_file("read_script_test_1.sql");

        auto file = std::fopen(dir(test_file).c_str(), "w");
        std::fwrite(script_create.data(), sizeof(char), script_create.size(), file);
        std::fwrite(script_insert.data(), sizeof(char), script_insert.size(), file);
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        auto commands = initializer.readCommands(dir(test_file));

        REQUIRE(commands.size() == 2);
    }

    SECTION("read empty script files")
    {
        ScopedDir dir(USER_PATH("scripts"));
        std::string test_file("read_empyty_1.sql");

        auto file = std::fopen(dir(test_file).c_str(), "w");
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        auto commands = initializer.readCommands(dir(test_file));

        REQUIRE(commands.size() == 0);
    }

    SECTION("read script file with comment")
    {
        ScopedDir dir(USER_PATH("read_script_file_with_comment"));
        std::string test_file("test_01.sql");

        auto file = std::fopen(dir(test_file).c_str(), "w");
        std::fwrite(script_comment.data(), sizeof(char), script_comment.size(), file);
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        auto commands = initializer.readCommands(dir(test_file));

        REQUIRE(commands.size() == 2);
    }

    SECTION("execute valid script")
    {
        ScopedDir dir(USER_PATH("execute_valid_script"));
        std::string test_file("test_01.sql");

        auto file = std::fopen(dir(test_file).c_str(), "w");
        std::fwrite(script_create.data(), sizeof(char), script_create.size(), file);
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        bool r = initializer.run(dir());

        REQUIRE(r == true);
    }

    SECTION("execute invalid script")
    {
        ScopedDir dir(USER_PATH("execute_invalid_script"));
        std::string test_file("test_1.sql");

        auto file = std::fopen(dir(test_file).c_str(), "w");
        std::fwrite(script_invalid.data(), sizeof(char), script_invalid.size(), file);
        std::fclose(file);

        Database db(dir("test.db").c_str());
        DatabaseInitializer initializer(&db);
        bool result = initializer.run(dir());

        REQUIRE(result == false);
    }

    Database::deinitialize();
}
