local recovery = require('recovery')
local consts = require('consts')

local paths = {}

local user_dir = recovery.sys.user()
local system_dir = recovery.sys.source_slot()

paths.data_dir = system_dir .. "/data"
paths.db_dir = system_dir .. "/db"
paths.db_factory_dir = paths.db_dir .. "/factory"
paths.temp_dir = user_dir .. "/temp"
paths.update_dir = user_dir .. "/temp/update"
paths.migration_scripts_dir = paths.db_dir .. "/migration"

paths.version_file = system_dir .. "/" .. consts.version_file
paths.backup_file = paths.temp_dir .. "/backup.tar"
paths.file_indexer_cache = system_dir .. "/data/.directory_is_indexed"

return paths
