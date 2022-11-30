local product_updater = {}

local paths = require('paths')
local helpers = require('helpers')
local lfs = require('lfs')
local paths = require('paths')

function product_updater.is_migration_needed()
    local current_version = helpers.get_os_version(paths.version_file)
    local version = "1.5.0"
    assert(current_version >= version, string.format("Updating from version '%s' not supported", current_version))
    return current_version == version
end

function product_updater.execute()
    local old_music_dir = "/user/music"
    local new_music_dir = "/user/app/music_player"
    local old_battery_config = "/user/batteryFuelGaugeConfig.cfg"
    local new_battery_config = paths.data_dir .. "/batteryFuelGaugeConfig.cfg"

    assert(helpers.mkdirp(new_music_dir))
    helpers.copy_dir(old_music_dir, new_music_dir)
    helpers.rmdir(old_music_dir)
    helpers.copy_file(old_battery_config,new_battery_config)
    os.remove(old_battery_config)

end

return product_updater

