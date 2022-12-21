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
local version_file = unpacked_backup_dir .. "/" .. consts.version_file
local legacy_version_file = unpacked_backup_dir .. "/" .. consts.legacy_version_file

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

local function build_db_set(file)
    local contents = helpers.read_whole_file(file)
    local root = json.decode(contents)
    local set = {}
    for _, entry in pairs(root.databases) do
        set[entry.name] = tonumber(entry.version)
    end
    return set
end

local function get_legacy_db_set()
    local set = {
        ["calllog"] = 0,
        ["sms"] = 0,
        ["events"] = 0,
        ["settings_v2"] = 0,
        ["notes"] = 0,
        ["custom_quotes"] = 0,
        ["predefined_quotes"] = 0,
        ["contacts"] = 0,
        ["alarms"] = 0,
        ["notifications"] = 0,
        ["multimedia"] = 0
    }
    return set
end

local function perform_db_migration()
    print("Performing database migration")
    local dbset = {}
    if helpers.exists(version_file) then
        dbset = build_db_set(version_file)
    else
        assert(helpers.exists(legacy_version_file))
        print("Legacy backup file, assuming legacy databases set")
        dbset = get_legacy_db_set()
    end

    local result = migration.migrate(unpacked_backup_dir, paths.migration_scripts_dir, dbset)
    assert(result == migration.retcode.OK, string.format("Database migration process failed with %d", result))
end

local function sync_databases()
    print("Syncing databases:")
    print(string.format("Replacing old databases: '%s' with the ones from '%s'", paths.db_dir, unpacked_backup_dir))

    helpers.rm_files_from_dir(paths.db_dir)
    helpers.copy_dir(unpacked_backup_dir, paths.db_dir)

    local version_file_path = paths.db_dir .. "/" .. consts.version_file
    if not helpers.exists(version_file_path) then
        version_file_path = paths.db_dir .. "/" .. consts.legacy_version_file
    end
    assert(os.remove(version_file_path))
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
