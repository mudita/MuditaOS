local paths = require('paths')
local consts = require('consts')
local helpers = require('helpers')
local recovery = require('recovery')

local update_product = {}

function update_product.execute()
    local old_music_dir = recovery.sys.user() .. "/music"
    local new_music_dir = recovery.sys.user() .. "/media/app/relaxation"

    assert(helpers.mkdirp(new_music_dir))
    helpers.move_dir(old_music_dir, new_music_dir)
    helpers.rmdir(old_music_dir)
end

return update_product
