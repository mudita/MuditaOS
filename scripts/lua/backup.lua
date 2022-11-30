local helpers = require('helpers')
local recovery = require('recovery')
local paths = require('paths')
local tar = require('ltar')
local lfs = require('lfs')
local backup = {}

-- Match only files with '.db' extensions and omit such files inside subdirectories
local match_db_files = '^[^%/]*%.db$'

backup.script_name = "backup.lua"
backup.img_in_progress = "assets/gui_image_backup_in_progress.bin"
backup.img_success = "assets/gui_image_backup_success.bin"
backup.img_failure = "assets/gui_image_backup_failed.bin"

local function check_available_space()
    local db_size = helpers.dir_size_filtered(paths.db_dir, match_db_files)
    local version_size = lfs.attributes(paths.version_file, 'size')
    local available_space = recovery.sys.free_space(paths.user_dir)
    -- Multiply the result by two due to the internal padding inside tar
    local required_space = (db_size + version_size) * 2

    print(string.format("Checking disk space:\nNeeded space: %d bytes, available space: %d bytes", required_space,
        available_space))

    assert(available_space > required_space, "Not enough free space on user disk")
end

local function create_temp_dir()
    if not helpers.exists(paths.temp_dir) then
        print(string.format("'%s' does not exist, creating", paths.temp_dir))
        lfs.mkdir(paths.temp_dir)
    end
end

local function create_backup_package()
    print(string.format("Creating backup package from the contents of '%s' directory and saving it into '%s'",
        paths.db_dir, paths.backup_file))
    tar.create_from_path_regex(paths.db_dir, paths.backup_file, match_db_files)
    tar.append_file(paths.version_file, paths.backup_file)
end

function backup.execute()
    check_available_space()
    create_temp_dir()
    create_backup_package()
end

return backup

