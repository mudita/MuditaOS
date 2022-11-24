-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
--- Database migration scripts
-- @module migration
local sqlite3 = require("lsqlite3complete")
local lfs = require("lfs")
local helpers = require("helpers")

local up_script = "up.sql"
local down_script = "down.sql"

migration = {}

--- Return codes
-- @table retcode 
migration.retcode = {
    OK = 0,
    ALREADY_UP_TO_DATE = 1,
    WRONG_TARGET_VERSION = 2
}

local function build_script_array(path, db_name, filename)
    local scripts = {}
    for file in lfs.dir(path .. "/" .. db_name) do
        if file ~= "." and file ~= ".." and tonumber(file) then
            local f = path .. '/' .. db_name .. '/' .. file .. '/' .. filename
            local attr = lfs.attributes(f)
            assert(type(attr) == "table")
            scripts[tonumber(file)] = helpers.read_whole_file(f)
        end
    end
    return scripts
end

local function build_database_path(path, db_name)
    return path .. "/" .. db_name .. ".db"
end

local function db_exec(file, script, version)
    local db = assert(sqlite3.open(file), string.format("file: %s", file))
    assert(db:exec(script) == sqlite3.OK, string.format("script:\n%s\n", script))
    assert(db:exec(string.format("PRAGMA user_version=%u;", version)) == sqlite3.OK,
        string.format("version: %d", version))
    db:close()
end

local function read_db_version(file)
    local db = assert(sqlite3.open(file))
    local stmt = assert(db:prepare("PRAGMA user_version;"))
    local ret = {}
    for v in stmt:urows() do
        ret = v
    end
    db:close()
    return ret
end

local function db_migrate_up(db_path, scripts, target_version)
    current_version = read_db_version(db_path)

    if current_version == target_version then
        return migration.retcode.ALREADY_UP_TO_DATE
    end

    if current_version > target_version then
        return migration.retcode.WRONG_TARGET_VERSION
    end

    for v = current_version, target_version - 1 do
        db_exec(db_path, scripts[v + 1], v + 1)
    end
    return migration.retcode.OK
end

local function db_migrate_down(db_path, scripts, target_version)
    current_version = read_db_version(db_path)

    if current_version == target_version then
        return migration.retcode.ALREADY_UP_TO_DATE
    end

    if current_version < target_version then
        return migration.retcode.WRONG_TARGET_VERSION
    end

    for v = current_version, target_version + 1, -1 do
        db_exec(db_path, scripts[v], v - 1)
    end
    return migration.retcode.OK
end

local function print_db_set(db_set)
    print("database set:")
    for name, version in pairs(db_set) do
        print("\tname: " .. name)
        print("\ttarget version: " .. version)
    end
end

local function validate_inputs(migration_dir, db_dir)
    assert(helpers.exists(migration_dir), "Migrations directory does not exist")
    assert(helpers.exists(db_dir), "Databases directory does not exist")
    return true
end

local function migrate(db_path, scripts_up, scripts_down, target_version)
    local db_version = read_db_version(db_path)
    if db_version > target_version then
        return migration.down(db_path, scripts_down, target_version)
    end
    if db_version < target_version then
        return migration.up(db_path, scripts_up, target_version)
    end
    return migration.retcode.OK
end

--- Perform database migration from lower to higher version
-- @function up
-- @param db_path path to the database we want to perform migration on
-- @param scripts array of {""}
-- @param target_version database version we want to migrate in to
-- @return @{retcode}
function migration.up(db_path, scripts, target_version)
    return db_migrate_up(db_path, scripts, target_version)
end

--- Perform database migration from higher to lower version
-- @function down
-- @param db_path path to the database we want to perform migration on
-- @param scripts array of {""}
-- @param target_version database version we want to migrate in to
-- @return @{retcode}
function migration.down(db_path, scripts, target_version)
    return db_migrate_down(db_path, scripts, target_version)
end

--- Read the database version
-- @function get_db_version
-- @param db_path path to the database
-- @return database version number
function migration.get_db_version(db_path)
    return read_db_version(db_path)
end

--- Perform databases migration(up/down) automatically
-- @function migrate
-- @param db_dir location of the databases
-- @param scripts_dir location of the database migration scripts 
-- @param db_set array of {<"database_name"> = <db_target_version>} entries
-- @return @{retcode}
function migration.migrate(db_dir, scripts_dir, db_set)
    print(string.format("migrations scripts directory: '%s'", scripts_dir))
    print(string.format("databases directory: '%s'", db_dir))
    print_db_set(db_set)

    for name, version in pairs(db_set) do
        local database_path = build_database_path(db_dir, name)
        local scripts_up = build_script_array(scripts_dir, name, up_script)
        local scripts_down = build_script_array(scripts_dir, name, down_script)
        local ret = migrate(database_path, scripts_up, scripts_down, version)
        if ret ~= migration.retcode.OK then
            return ret
        end
    end

    return migration.retcode.OK
end

return migration
