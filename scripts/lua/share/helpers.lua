-- Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
--- helper scripts
-- @module helpers
local lfs = require('lfs')
local json = require('lunajson')

local helpers = {}

local function dirtree(dir)
    assert(dir and dir ~= "", "directory parameter is missing or empty")
    if string.sub(dir, -1) == "/" then
        dir = string.sub(dir, 1, -2)
    end

    local function yieldtree(dir)
        for entry in lfs.dir(dir) do
            if entry ~= "." and entry ~= ".." then
                entry = dir .. "/" .. entry
                local attr = lfs.attributes(entry)
                coroutine.yield(entry, attr)
                if attr.mode == "directory" then
                    yieldtree(entry)
                end
            end
        end
    end

    return coroutine.wrap(function()
        yieldtree(dir)
    end)
end

local function ends_with(str, ending)
    return ending == "" or str:sub(-#ending) == ending
end

local function starts_with(str, start)
    return str:sub(1, #start) == start
end

local function build_path(prefix, name)
    if not ends_with(prefix, '/') then
        prefix = prefix .. '/'
    end
    if starts_with(name, '/') then
        name = name:sub(2, -1)
    end
    return prefix .. name
end

local function strip_from_prefix(prefix, path)
    local name = path:gsub(prefix, "")
    name = name:sub(2, -1)
    return name
end

local function basedir(p)
    return p:gsub('[^\\/]+[\\/]?$', '')
end

--- Copy file
-- @function copy_file
-- @param filename_in input file
-- @param filename_out output file
function helpers.copy_file(filename_in, filename_out)
    local size = 1024 * 512
    local size_in = lfs.attributes(filename_in, "size")
    local fd_in = assert(io.open(filename_in, "r"))
    local fd_out = assert(io.open(filename_out, "w"))
    while true do
        local block = fd_in:read(size)
        if not block then
            break
        end
        assert(fd_out:write(block))
    end
    fd_in:close()
    fd_out:close()
end

--- Remove directory and its contents
-- @function rmdir
-- @param dir directory to remove
function helpers.rmdir(dir)
    for file in lfs.dir(dir) do
        local file_path = dir .. "/" .. file
        if file ~= "." and file ~= ".." then
            if lfs.attributes(file_path, "mode") == "file" then
                assert(os.remove(file_path))
            elseif lfs.attributes(file_path, "mode") == "directory" then
                helpers.rmdir(file_path)
            end
        end
    end
    lfs.rmdir(dir)
end

--- Remove directory content
-- @function rmdir_content
-- @param dir directory path
function helpers.rmdir_content(dir)
    for file in lfs.dir(dir) do
        local file_path = dir .. "/" .. file
        if file ~= "." and file ~= ".." then
            if lfs.attributes(file_path, "mode") == "file" then
                assert(os.remove(file_path))
            elseif lfs.attributes(file_path, "mode") == "directory" then
                helpers.rmdir(file_path)
            end
        end
    end
end

--- Remove all files from directory without touching internal directories
-- @function rm_files_from_dir
-- @param dir directory to remove files from
function helpers.rm_files_from_dir(dir)
    for file in lfs.dir(dir) do
        local file_path = dir .. "/" .. file
        if file ~= "." and file ~= ".." then
            if lfs.attributes(file_path, "mode") == "file" then
                assert(os.remove(file_path))
            end
        end
    end
end

--- Copy directory recursively
-- @function copy_dir
-- @param from source directory
-- @param where target directory
function helpers.copy_dir(from, where)
    for filename, attr in dirtree(from) do
        local name = filename:gsub(from, "")
        if attr.mode == "directory" then
            assert(lfs.mkdir(build_path(where, name)))
        else
            helpers.copy_file(filename, build_path(where, name))
        end
    end
end

--- Copy directory recursively using regex filter
-- @function copy_dir_filtered
-- @param from source directory
-- @param where target directory
-- @param matcher regex expression
function helpers.copy_dir_filtered(from, where, filter)
    for filename, attr in dirtree(from) do
        local name = strip_from_prefix(from, filename)
        if name:match(filter) then
            if attr.mode == "directory" then
                assert(lfs.mkdir(build_path(where, name)))
            else
                helpers.copy_file(filename, build_path(where, name))
            end
        end
    end
end

--- Get the size of specified directory using regex filter
-- @function dir_size_filtered
-- @param path directory path
-- @param matcher regex expression
-- @return total size of directory in bytes
function helpers.dir_size_filtered(path, filter)
    local total_size = 0
    for filename, attr in dirtree(path) do
        local name = strip_from_prefix(path, filename)
        if name:match(filter) then
            total_size = total_size + attr.size
        end
    end
    return total_size
end

--- Get the size of specified directory
-- @function dir_size
-- @param path directory path
-- @return total size of directory in bytes
function helpers.dir_size(path)
    return helpers.dir_size_regex('.*')
end

--- Check if specified dir/file exists
-- @function exists
-- @param path directory or file path
-- @return true or false
function helpers.exists(path)
    local ret = lfs.attributes(path)
    return ret ~= nil
end

--- Read the whole file at once 
-- @function read_whole_file
-- @param file file path
-- @return file contents
function helpers.read_whole_file(file)
    local f = assert(io.open(file, "rb"))
    local content = assert(f:read("*a"))
    f:close()
    return content
end

--- Extract filename from filename path
-- @function get_filename
-- @param file file path
-- @return filename
function helpers.get_filename(file)
    return file:match("^.+/(.+)$")
end

--- Extract file extension from filename
-- @function get_file_extension
-- @param file file path
-- @return file extension
function helpers.get_file_extension(file)
    return file:match("^.+(%..+)$")
end

--- Create directory and all required subdirectories
-- @function mkdirp
-- @param file file path
function helpers.mkdirp(p)
    if lfs.attributes(p, 'mode') == 'directory' then
        return nil, 'already exists'
    end

    local b = basedir(p)
    if #b > 0 and lfs.attributes(b, 'mode') ~= 'directory' then
        local r, m = helpers.mkdirp(b)
        if not r then
            return r, m
        end
    end
    return lfs.mkdir(p)
end

--- Get OS version from the 'version.json'
-- @function get_os_version
-- @param file file path
function helpers.get_os_version(file)
    local contents = helpers.read_whole_file(file)
    local root = json.decode(contents)
    return root.os.version
end

return helpers
