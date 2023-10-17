-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
package.path = package.path .. ";../?.lua;../../share/?.lua;../../share/?/?.lua"
local lu = require("luaunit")
local sqlite = require("lsqlite3complete")
local lfs = require("lfs")
local migration = require("migration")

local test_db_schema = [[
    CREATE TABLE IF NOT EXISTS test_1
   (
    _id         INTEGER PRIMARY KEY,
   timestamp   TEXT,
   duration    INTEGER
   );"
   ]]

function spawn_db(name, version, schema)
    os.remove(name)
    local db = sqlite.open(name)
    db:exec(schema)
    db:exec(string.format("PRAGMA user_version=%u;", version))
    db:close()
end

function spawn_script(name, dir, db_name, version, body)
    lfs.mkdir(dir .. "/" .. db_name)
    lfs.mkdir(dir .. "/" .. db_name .. "/" .. version)
    local fd = io.open(dir .. "/" .. db_name .. "/" .. version .. "/" .. name, "w")
    fd:write(body)
    fd:close()
end

function test_migration_up_success()
    scripts = {}
    scripts[1] = "ALTER TABLE test_1 ADD new_column TEXT;"
    scripts[2] = "ALTER TABLE test_1 ADD new_column2 TEXT;"
    scripts[3] = "ALTER TABLE test_1 ADD new_column3 TEXT;"
    scripts[4] = "ALTER TABLE test_1 ADD new_column4 TEXT;"
    scripts[5] = "ALTER TABLE test_1 ADD new_column5 TEXT;"

    --- Migrate from version 0 to 1
    spawn_db("databases/test_1.sql", 0, test_db_schema)
    lu.assertEquals(migration.up("databases/test_1.sql", scripts, 1), migration.retcode.OK)
    lu.assertEquals(migration.get_db_version("databases/test_1.sql"), 1)

    -- Migrate from version 1 to 2
    lu.assertEquals(migration.up("databases/test_1.sql", scripts, 2), migration.retcode.OK)
    lu.assertEquals(migration.get_db_version("databases/test_1.sql"), 2)

    --- Migrate from version 0 to 5
    spawn_db("databases/test_1.sql", 0, test_db_schema)
    lu.assertEquals(migration.up("databases/test_1.sql", scripts, 5), migration.retcode.OK)
    lu.assertEquals(migration.get_db_version("databases/test_1.sql"), 5)
end

function test_migration_up_db_errors()
    scripts = {}

    --- Trigger DB error by trying to operate on empty database
    spawn_db("databases/test_1.sql", 1, "")
    scripts[1] = "ALTER TABLE test_1 ADD new_column TEXT;"
    lu.assertError(migration.up, "databases/test_1.sql", scripts, 2)

    --- Target version set to the same value as the current DB version
    spawn_db("databases/test_1.sql", 1, test_db_schema)
    lu.assertEquals(migration.up("databases/test_1.sql", scripts, 1), migration.retcode.ALREADY_UP_TO_DATE)

    --- Target version set to the lower number than the current DB version
    spawn_db("databases/test_1.sql", 2, test_db_schema)
    lu.assertEquals(migration.up("databases/test_1.sql", scripts, 1), migration.retcode.WRONG_TARGET_VERSION)
end

function test_migration_down_success()
    scripts = {}
    scripts[1] = "ALTER TABLE test_1 ADD new_column1 TEXT;"
    scripts[2] = "ALTER TABLE test_1 ADD new_column2 TEXT;"
    scripts[3] = "ALTER TABLE test_1 ADD new_column3 TEXT;"
    scripts[4] = "ALTER TABLE test_1 ADD new_column4 TEXT;"
    scripts[5] = "ALTER TABLE test_1 ADD new_column5 TEXT;"

    --- Migrate from version 2 to 1
    spawn_db("databases/test_1.sql", 2, test_db_schema)
    lu.assertEquals(migration.down("databases/test_1.sql", scripts, 1), migration.retcode.OK)
    lu.assertEquals(migration.get_db_version("databases/test_1.sql"), 1)

    -- Migrate from version 1 to 0
    lu.assertEquals(migration.down("databases/test_1.sql", scripts, 0), migration.retcode.OK)
    lu.assertEquals(migration.get_db_version("databases/test_1.sql"), 0)

    --- Migrate from version 5 to 0
    spawn_db("databases/test_1.sql", 5, test_db_schema)
    lu.assertEquals(migration.down("databases/test_1.sql", scripts, 0), migration.retcode.OK)
    lu.assertEquals(migration.get_db_version("databases/test_1.sql"), 0)
end

function test_migration_down_errors()
    scripts = {}

    --- Trigger DB error by trying to operate on empty database
    spawn_db("databases/test_1.sql", 2, "")
    scripts[1] = "ALTER TABLE test_1 ADD new_column TEXT;"
    lu.assertError(migration.down, "databases/test_1.sql", scripts, 1)

    --- Target version set to the same value as the current DB version
    spawn_db("databases/test_1.sql", 1, test_db_schema)
    lu.assertEquals(migration.down("databases/test_1.sql", scripts, 1), migration.retcode.ALREADY_UP_TO_DATE)

    --- Target version set to the higher number than the current DB version
    spawn_db("databases/test_1.sql", 2, test_db_schema)
    lu.assertEquals(migration.down("databases/test_1.sql", scripts, 3), migration.retcode.WRONG_TARGET_VERSION)
end

function test_automatic_migration()
    spawn_db("databases/test_1.db", 0, test_db_schema)
    spawn_db("databases/test_2.db", 0, test_db_schema)
    spawn_script("up.sql", "migrations", "test_1", 1, "insert into test_1 (duration) values(100);")
    spawn_script("up.sql", "migrations", "test_1", 2, "insert into test_1 (duration) values(101);")
    spawn_script("up.sql", "migrations", "test_2", 1, "insert into test_1 (duration) values(100);")
    spawn_script("up.sql", "migrations", "test_2", 2, "insert into test_1 (duration) values(101);")
    spawn_script("down.sql", "migrations", "test_1", 1, "delete from test_1 where _id=1;")
    spawn_script("down.sql", "migrations", "test_1", 2, "delete from test_1 where _id=2;")
    spawn_script("down.sql", "migrations", "test_2", 1, "delete from test_1 where _id=1;")
    spawn_script("down.sql", "migrations", "test_2", 2, "delete from test_1 where _id=2;")

    migration.migrate("databases", "migrations", {
        test_1 = 0,
        test_2 = 0
    })
    lu.assertEquals(migration.get_db_version("databases/test_1.db"), 0)
    lu.assertEquals(migration.get_db_version("databases/test_2.db"), 0)

    migration.migrate("databases", "migrations", {
        test_1 = 2,
        test_2 = 2
    })
    lu.assertEquals(migration.get_db_version("databases/test_1.db"), 2)
    lu.assertEquals(migration.get_db_version("databases/test_2.db"), 2)

    migration.migrate("databases", "migrations", {
        test_1 = 1,
        test_2 = 1
    })
    lu.assertEquals(migration.get_db_version("databases/test_1.db"), 1)
    lu.assertEquals(migration.get_db_version("databases/test_2.db"), 1)
end

os.exit(lu.LuaUnit.run())
