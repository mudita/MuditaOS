local helpers = require('helpers')
local recovery = require('recovery')
local paths = require('paths')
local consts = require('consts')
local ltar = require('ltar')
local lfs = require('lfs')
local json = require('lunajson')
local migration = require('migration')
local restore = {}

local unpacked_backup_dir = paths.temp_dir .. "/backup"

restore.script_name = "restore.lua"
restore.img_in_progress = "assets/gui_image_restore_in_progress.bin"
restore.img_success = "assets/gui_image_restore_success.bin"
restore.img_failure = "assets/gui_image_restore_failed.bin"

local function check_available_space()
    local backup_size = lfs.attributes(paths.backup_file, 'size')
    local available_space = recovery.sys.free_space(recovery.sys.user())

    print(string.format("Checking disk space:\nNeeded space: %d bytes, available space: %d bytes", backup_size,
        available_space))

    assert(available_space > backup_size, "Not enough free space on user disk")
end

local function unpack_backup()
    if helpers.exists(unpacked_backup_dir) then
        print(string.format("Directory '%s' exists, removing", unpacked_backup_dir))
        helpers.rmdir(unpacked_backup_dir)
    end
    lfs.mkdir(unpacked_backup_dir)
    print(string.format("Unpacking backup file '%s' to '%s'", paths.backup_file, unpacked_backup_dir))
    ltar.unpack(paths.backup_file, unpacked_backup_dir)
end

local function get_db_array_from_file(file)
    local contents = helpers.read_whole_file(file)
    local root = json.decode(contents)
    local set = {}
    for _, entry in pairs(root.databases) do
        set[entry.name] = tonumber(entry.version)
    end
    return set
end

local function get_db_array_from_path(path)
    local set = {}
    for file in lfs.dir(path) do
        local file_path = path .. "/" .. file
        if file ~= "." and file ~= ".." then
            if lfs.attributes(file_path, "mode") == "file" then
                set[helpers.strip_from_extension(file)] = true;
            end
        end
    end
    return set
end

local function build_db_set()
    local system_db_set = get_db_array_from_file(paths.version_file)
    local backup_db_set = get_db_array_from_path(unpacked_backup_dir)
    local set = {}
    for name, version in pairs(system_db_set) do
        if backup_db_set[name] then
            set[name] = tonumber(version)
        end
    end
    return set
end

local function perform_db_migration()
    print("Performing database migration")

    local result = migration.migrate(unpacked_backup_dir, paths.migration_scripts_dir, build_db_set())
    assert(result == migration.retcode.OK, string.format("Database migration process failed with %d", result))
end

local function sync_databases()
    print("Syncing databases:")

    for name, _ in pairs(build_db_set()) do
        local destination = paths.db_dir .. "/" .. name .. ".db"
        local source = unpacked_backup_dir .. "/" .. name .. ".db"
        print(string.format("Replacing '%s' with '%s'", destination, source));
        assert(os.remove(destination))
        helpers.copy_file(source, destination)
    end
end

local function remove_cache()
    if helpers.exists(paths.file_indexer_cache) then
        print(string.format("Removing cache file '%s'", paths.file_indexer_cache))
        assert(os.remove(paths.file_indexer_cache))
    end
end

local function cleanup()
    if helpers.exists(unpacked_backup_dir) then
        helpers.rmdir(unpacked_backup_dir)
    end
end

function restore.execute()
    check_available_space()
    unpack_backup()
    perform_db_migration()
    sync_databases()
    remove_cache()
    cleanup()
end

return restore
