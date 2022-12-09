--- LUA support for TAR packages
-- @module ltar
local microtar = require("lmicrotar")
local lfs = require("lfs")

local tar = {}

local function strip_from_prefix(prefix, filename)
    local name = filename:gsub(prefix, "")
    name = name:sub(2, -1)
    return name
end

local function get_filename(file)
    return file:match("^.+/(.+)$")
end

local function basedir(p)
    return p:gsub('[^\\/]+[\\/]?$', '')
end

local function mkdirp(p)
    if lfs.attributes(p, 'mode') == 'directory' then
        return nil, 'already exists'
    end

    local b = basedir(p)
    if #b > 0 and lfs.attributes(b, 'mode') ~= 'directory' then
        local r, m = mkdirp(b)
        if not r then
            return r, m
        end
    end
    return lfs.mkdir(p)
end

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

local function write_tarfile_chunks(handle, fd)
    local size = 1024 * 512
    collectgarbage()
    while true do
        local block = fd:read(size)
        if not block then
            break
        end
        handle:write_data(block, block:len())
    end
end

local function read_tarfile_chunks(handle, fd, total_size)
    local block_size = 1024 * 512
    local to_read = {};

    collectgarbage()
    while total_size > 0 do
        if total_size > block_size then
            to_read = block_size
        else
            to_read = total_size
        end
        fd:write(handle:read_data(to_read))
        total_size = total_size - to_read
    end
end

--- Create empty tar file
-- @function create
-- @param path where to put newly created tar file
-- @return tar handle or nil
function tar.create(path)
    Handle = {
        handle = microtar.open(path, "w"),
        add_file = function(self, filename)
            local size = lfs.attributes(filename, "size")
            local fd = io.open(filename, "r")
            self.handle:write_file_header(filename, size)
            write_tarfile_chunks(self.handle, fd)
            fd:close()
        end,
        add_directory = function(self, path)
            self.handle:write_dir_header(path)
        end,
        close = function(self)
            self.handle:close()
        end
    }
    return Handle
end

--- Iterate over contents of tar file
-- @function iter_by_handle
-- @param handle tar handle create by @{create}
function tar.iter_by_handle(handle)
    return function()
        local f = handle:read_header()
        if f then
            handle:next()
            return f
        end
    end
end

--- Iterate over contents of tar file
-- @function iter_by_path
-- @param path path to tar file
function tar.iter_by_path(path)
    local handle = microtar.open(path)
    return function()
        local f = handle:read_header()
        if f then
            handle:next()
            return f
        end
    end
end

--- Pack contents of specified dir to tar file
-- @function create_from_path
-- @param path directory 
-- @param where where to save tar file 
function tar.create_from_path(path, where)
    tar.create_from_path_match(path, where, ".*")
end

--- Pack contents of specified dir to tar file using regex
-- @function create_from_path_regex
-- @param path directory 
-- @param where where to save tar file 
-- @param matcher regex expression
function tar.create_from_path_regex(path, where, matcher)
    local handle = microtar.open(where, "w")
    for filename, attr in dirtree(path) do
        local name = strip_from_prefix(path, filename)
        if name:match(matcher) then
            if attr.mode == "directory" then
                handle:write_dir_header(name)
            else
                handle:write_file_header(name, attr.size)
                local fd = io.open(filename, "r")
                write_tarfile_chunks(handle, fd)
                fd:close()
            end
        end
    end
    handle:close()
end

--- Unpack tar file to specified directory
-- @function unpack
-- @param path directory 
-- @param where where to store unpacked files 
function tar.unpack(path, where)
    local handle = microtar.open(path)
    local header = handle:read_header()
    while header do
        if header.type == microtar.TDIR then
            mkdirp(where .. "/" .. header.name)
        elseif header.type == microtar.TREG then
            local fd = io.open(where .. "/" .. header.name, "w")
            read_tarfile_chunks(handle, fd, header.size)
            fd:close()
        end
        handle:next()
        header = handle:read_header()
    end
    handle:close()
end

--- Append directory to already existing tar file
-- @function append
-- @param path directory 
-- @param where location of already existing tar file 
function tar.append(path, where)
    local handle = microtar.open(where, "a")
    for filename, attr in dirtree(path) do
        if attr.mode == "directory" then
            handle:write_dir_header(filename)
        else
            handle:write_file_header(filename, attr.size)
            local fd = io.open(filename, "r")
            write_tarfile_chunks(handle, fd)
            fd:close()
        end
    end
    handle:close()
end

--- Append file to already existing tar file
-- @function append_file
-- @param path file path 
-- @param where location of already existing tar file 
function tar.append_file(path, where)
    local filename = get_filename(path)
    local size = lfs.attributes(path, 'size')
    local handle = microtar.open(where, "a")
    handle:write_file_header(filename, size)
    local fd = io.open(path, "r")
    write_tarfile_chunks(handle, fd)
    fd:close()
    handle:close()
end

function tar.find(tar_path, what)
    local handle = microtar.open(tar_path)
    local stats = handle:find(what)
    if stats ~= nil then
        return handle:read_data()
    end
    return nil
end

return tar
