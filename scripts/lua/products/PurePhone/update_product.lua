local paths = require('paths')
local consts = require('consts')
local helpers = require('helpers')
local recovery = require('recovery')

local update_product = {}

function update_product.execute()
    local old_music_dir = recovery.sys.user() .. "/music"
    local new_music_dir = recovery.sys.user() .. "/media/app/music_player"
    local old_battery_config = recovery.sys.user() .. "/batteryFuelGaugeConfig.cfg"
    local new_battery_config = paths.target.var_dir .. "/batteryFuelGaugeConfig.cfg"

    assert(helpers.mkdirp(new_music_dir))
    helpers.move_dir(old_music_dir, new_music_dir)
    helpers.rmdir(old_music_dir)
    if helpers.exists(old_battery_config) then
        helpers.copy_file(old_battery_config, new_battery_config)
    end
end

return update_product
