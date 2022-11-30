local helpers = require('helpers')
local paths = require('paths')

local factory = {}

factory.script_name = "factory.lua"
factory.img_in_progress = "assets/gui_image_factory_reset_in_progress.bin"
factory.img_success = "assets/gui_image_factory_reset_success.bin"
factory.img_failure = "assets/gui_image_factory_reset_failed.bin"

local function remove_old_databases()
    print(string.format("Removing old databases from '%s'", paths.db_dir))
    helpers.rm_files_from_dir(paths.db_dir)
end

local function copy_clean_databases()
    print(string.format("Copying clean databases from '%s' to '%s'", paths.db_factory_dir, paths.db_dir))
    helpers.copy_dir(paths.db_factory_dir, paths.db_dir)
end

local function remove_cache()
    if helpers.exists(paths.file_indexer_cache) then
        print(string.format("Removing cache file '%s'", paths.file_indexer_cache))
        assert(os.remove(paths.file_indexer_cache))
    end
end

function factory.execute()
    remove_old_databases()
    copy_clean_databases()
    remove_cache()
end

return factory
