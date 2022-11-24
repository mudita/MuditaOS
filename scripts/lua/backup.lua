local helpers = require('helpers')
local recovery = require('recovery')
local tar = require('ltar')
local lfs = require('lfs')
local backup = {}

backup.script_name = "backup.lua"
backup.img_in_progress = "assets/gui_image_backup_in_progress.bin"
backup.img_success = "assets/gui_image_backup_success.bin"
backup.img_failure = "assets/gui_image_backup_failed.bin"

function backup.execute()
end

return backup

