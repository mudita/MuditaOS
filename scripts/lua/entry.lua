local helpers = require('helpers')
local paths = require('paths')
local rec = require('recovery')
local backup = require('backup')
local restore = require('restore')
local update = require('update')
local factory = require('factory')

local scripts = {}
scripts[rec.sys.boot_reason_codes.backup] = backup
scripts[rec.sys.boot_reason_codes.restore] = restore
scripts[rec.sys.boot_reason_codes.update] = update
scripts[rec.sys.boot_reason_codes.factory] = factory

--- Display image
-- @function display_image
-- @param path path to raw image data
local function display_image(path)
    local raw_data = helpers.read_whole_file(path)
    rec.gui.clear()
    rec.gui.display_raw_img(600, 480, raw_data)
    -- Give some time to an user to view the displayed image
    rec.sys.sleep(5)
end

local function print_recovery_info()
    print(string.format("PureRecovery version: %s, branch: %s, revision: %s", rec.version(), rec.branch(),
        rec.revision()))
end

local function print_boot_reason()
    print(string.format("Boot reason: %s", rec.sys.boot_reason_str()))
end

local function generate_report_file(boot_reason_str, success, message)
    local file_path = paths.temp_dir .. "/recovery_status.json"
    local body = string.format(
        "{\"version\": \"%s\",\"branch\": \"%s\",\"revision\": \"%s\",\"operation\": \"%s\",\"successful\": %s,\"message\": \"%s\"}",
        rec.version(), rec.branch(), rec.revision(), boot_reason_str, tostring(success), message)
    local fd = assert(io.open(file_path, 'w'))
    fd:write(body)
    fd:close()
end

local function handle_boot_reason()
    local boot_reason = rec.sys.boot_reason()
    local boot_reason_str = rec.sys.boot_reason_str()
    rec.sys.set_boot_reason(rec.sys.boot_reason_codes.os)
    return boot_reason, boot_reason_str
end

local function handle_script_success(boot_reason)
    display_image(scripts[boot_reason].img_success)
    print("Finished successfully")
end

local function handle_script_failure(boot_reason, message)
    display_image(scripts[boot_reason].img_failure)
    print(message)
end

local function handle_exit(boot_reason_str, status, message)
    generate_report_file(boot_reason_str, status, message)
    rec.gui.clear()
end

local function invoke_script(boot_reason)
    print(string.format("Executing '%s' script...", scripts[boot_reason].script_name))
    display_image(scripts[boot_reason].img_in_progress)

    local status, message = pcall(scripts[boot_reason].execute)
    if status then
        handle_script_success(boot_reason)
        return true, "ok"
    else
        handle_script_failure(boot_reason, message)
        return false, message
    end
end

print_recovery_info()
print_boot_reason()
local boot_reason, boot_reason_str = handle_boot_reason()
local status, message = invoke_script(boot_reason)
handle_exit(boot_reason_str, status, message)

