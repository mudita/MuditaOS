local recovery = require('recovery')
local consts = require('consts')

local paths = {}

local user_dir = recovery.sys.user()
local system_dir = recovery.sys.source_slot()
local target_dir = recovery.sys.target_slot()

paths.var_dir = system_dir .. "/var"
paths.data_dir = system_dir .. "/data"
paths.db_dir = system_dir .. "/db"
paths.db_factory_dir = paths.db_dir .. "/factory"
paths.temp_dir = user_dir .. "/temp"
paths.update_dir = user_dir .. "/temp/update"
paths.migration_scripts_dir = paths.db_dir .. "/migration"

local target = {}
target.var_dir = target_dir .. "/var"
target.data_dir = target_dir .. "/data"
target.db_dir = target_dir .. "/db"
target.db_factory_dir = target.db_dir .. "/factory"
target.migration_scripts_dir = target.db_dir .. "/migration"
target.file_indexer_cache = target.var_dir .. "/" .. consts.indexer_cache_file
paths.target = target

paths.version_file = system_dir .. "/" .. consts.version_file
paths.backup_file = paths.temp_dir .. "/backup.tar"
paths.update_file = paths.temp_dir .. "/update.tar"
paths.file_indexer_cache = paths.var_dir .. "/" .. consts.indexer_cache_file

return paths
