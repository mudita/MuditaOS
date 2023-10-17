local update = {}
local lfs = require('lfs')
local json = require('lunajson')
local paths = require('paths')
local consts = require('consts')
local helpers = require('helpers')
local recovery = require('recovery')
local migration = require('migration')

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
    local to = recovery.sys.target_slot() .. "/db"
    print(string.format("Copying databases from '%s' to '%s'", from, to))
    helpers.copy_dir_filtered(from, to, match_db_files)
end

local function create_directories()
    print("Creating 'log' and 'crash_dumps' directories")

    local target_dir = recovery.sys.target_slot()
    lfs.mkdir(target_dir .. "/log")
    lfs.mkdir(target_dir .. "/crash_dumps")
    lfs.mkdir(target_dir .. "/var")
end

local function migrate_db()
    local version_file = paths.update_dir .. "/" .. consts.version_file

    print("Performing database migration")
    local dbset = build_db_set(version_file)
    local result = migration.migrate(paths.target.db_dir, paths.target.migration_scripts_dir, dbset)
    assert(result == migration.retcode.OK, string.format("Database migration process failed with %d", result))
end

local function remove_cache()
    if helpers.exists(paths.target.file_indexer_cache) then
        print(string.format("Removing cache file '%s'", paths.target.file_indexer_cache))
        assert(os.remove(paths.target.file_indexer_cache))
    end
end

local function copy_var_directory()
    local from = paths.var_dir
    local to = paths.target.var_dir
    print(string.format("Copying '%s' to '%s'", from, to))
    helpers.copy_dir(from, to)

end

local function enter()
    -- Mark the current slot as successful 
    recovery.bootctrl.mark_as_successful()
    -- Mark the target slot as unbootable
    recovery.bootctrl.mark_as_unbootable(recovery.bootctrl.get_next_active())
end

local function exit()
    print("Finishing update process...")

    helpers.rmdir(paths.update_dir)
    os.remove(paths.update_file)

    -- Update the working directory to the newly updated scripts directory
    lfs.chdir(recovery.sys.target_slot() .. "/scripts")

    recovery.bootctrl.mark_as_bootable(recovery.bootctrl.get_next_active())
    recovery.bootctrl.mark_as_active(recovery.bootctrl.get_next_active())
    recovery.sys.set_os_boot_status(false)
end

function update.execute()
    enter()
    purge_target_slot()
    copy_update_package()
    copy_databases()
    create_directories()
    migrate_db()
    copy_var_directory()
    remove_cache()
    exit()
end

return update
