local helpers = require('helpers')
local recovery = require('recovery')

local factory = {}
local user_dir = recovery.sys.user()
local system_dir = recovery.sys.source_slot()
local db_dir = system_dir .. "/db"
local db_factory_dir = system_dir .. "/db/factory"
local file_indexer_cache = system_dir .. "/data/.directory_is_indexed"

factory.script_name = "factory.lua"
factory.img_in_progress = "assets/gui_image_factory_reset_in_progress.bin"
factory.img_success = "assets/gui_image_factory_reset_success.bin"
factory.img_failure = "assets/gui_image_factory_reset_failed.bin"

local function remove_old_databases()
    print(string.format("Removing old databases from '%s'", db_dir))
    helpers.rm_files_from_dir(db_dir)
end

local function copy_clean_databases()
    print(string.format("Copying clean databases from '%s' to '%s'", db_factory_dir, db_dir))
    helpers.copy_dir(db_factory_dir, db_dir)
end

local function remove_cache()
    if helpers.exists(file_indexer_cache) then
        print(string.format("Removing cache file '%s'", file_indexer_cache))
        assert(os.remove(file_indexer_cache))
    end
end

function factory.execute()
    remove_old_databases()
    copy_clean_databases()
    remove_cache()
end

return factory
