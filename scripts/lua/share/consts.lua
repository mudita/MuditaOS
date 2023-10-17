local consts = {}

consts.version_file = "version.json"
consts.indexer_cache_file = ".directory_is_indexed"

local match = {}

-- Match only files with '.db' extensions and omit such files inside subdirectories
match.only_db_files = '^[^%/]*%.db$'

consts.match = match
return consts
