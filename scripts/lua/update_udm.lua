local update = {}
local lfs = require('lfs')
local json = require('lunajson')
local paths = require('paths')
local consts = require('consts')
local helpers = require('helpers')
local recovery = require('recovery')
local migration = require('migration')
local update_product = require('update_product')

update.script_name = "update.lua"
update.img_in_progress = "assets/gui_image_update_in_progress.bin"
update.img_success = "assets/gui_image_update_success.bin"
update.img_failure = "assets/gui_image_update_failed.bin"

-- Match only files with '.db' extensions and omit such files inside subdirectories
local match_db_files = '^[^%/]*%.db$'

local function build_db_set(file)
    local contents = helpers.read_whole_file(file)
    local root = json.decode(contents)
    local set = {}
    for _, entry in pairs(root.databases) do
        set[entry.name] = tonumber(entry.version)
    end
    return set
end

local function purge_target_slot()
    local target_dir = recovery.sys.target_slot()
    print(string.format("Removing target slot content, '%s'", target_dir))
    helpers.rmdir_content(target_dir)
end

local function copy_update_package()
    local target_dir = recovery.sys.target_slot()
    print(string.format("Copying content of the update package '%s to '%s'", paths.update_dir, target_dir))
    helpers.copy_dir(paths.update_dir, target_dir)
end

local function copy_databases()
    local from = paths.db_dir
    local to = paths.target.db_dir
    print(string.format("Copying databases from '%s' to '%s'", from, to))
    helpers.copy_dir_filtered(from, to, match_db_files)
end

local function create_directories()
    print("Creating 'log', 'crash_dumps' and 'var' directories")

    local target_dir = recovery.sys.target_slot()
    lfs.mkdir(target_dir .. "/log")
    lfs.mkdir(target_dir .. "/crash_dumps")
    lfs.mkdir(target_dir .. "/var")
end

local function migrate_db()
    local target_dir = recovery.sys.target_slot()
    local version_file = paths.update_dir .. "/" .. consts.version_file
    local db_path = target_dir .. "/db"
    local migration_dir = db_path .. "/migration"

    print("Performing database migration")
    local dbset = build_db_set(version_file)
    local result = migration.migrate(db_path, migration_dir, dbset)
    assert(result == migration.retcode.OK, string.format("Database migration process failed with %d", result))
end

local function exit()
    print("Finishing update process")
    helpers.rmdir(paths.update_dir)
    os.remove(paths.update_file)
    recovery.bootctrl.mark_as_bootable(recovery.bootctrl.get_next_active())
    recovery.bootctrl.mark_as_active(recovery.bootctrl.get_next_active())
    recovery.sys.set_os_boot_status(false)
end

local function repartition_filesystem()
    print("Repartinioning 'MUDITAOS' and 'BACKUP' partitions and changing theirs labels")
    recovery.sys.repartition_fs()
end

local function user_remove_directories()
    print("->Removing backup, crash_dums, data, db, logs, tmp directories")
    if helpers.exists(recovery.sys.user() .. "/backup") then
        helpers.rmdir(recovery.sys.user() .. "/backup")
    end
    if helpers.exists(recovery.sys.user() .. "/crash_dumps") then
        helpers.rmdir(recovery.sys.user() .. "/crash_dumps")
    end
    if helpers.exists(recovery.sys.user() .. "/data") then
        helpers.rmdir(recovery.sys.user() .. "/data")
    end
    if helpers.exists(recovery.sys.user() .. "/db") then
        helpers.rmdir(recovery.sys.user() .. "/db")
    end
    if helpers.exists(recovery.sys.user() .. "/logs") then
        helpers.rmdir(recovery.sys.user() .. "/logs")
    end
    if helpers.exists(recovery.sys.user() .. "/tmp") then
        helpers.rmdir(recovery.sys.user() .. "/tmp")
    end
end

local function user_remove_files()
    print("->Removing files")
    helpers.rm_files_from_dir(recovery.sys.user())
end

local function user_create_directories()
    print("->Creating media, temp directories")
    lfs.mkdir(recovery.sys.user() .. "/media")
    lfs.mkdir(recovery.sys.user() .. "/temp")
end

local function user_product_specific()
    print("->Executing product specific steps")
    update_product.execute()
end

local function adjust_user_partition_layout()
    print("Adjusting user partition layout...")
    user_remove_directories()
    user_create_directories()
    user_product_specific()
    user_remove_files()
end

function update.execute()
    repartition_filesystem()
    purge_target_slot()
    copy_update_package()
    copy_databases()
    create_directories()
    migrate_db()
    adjust_user_partition_layout()
    exit()
end

return update
