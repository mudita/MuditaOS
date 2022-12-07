package.path = package.path .. ";../?.lua;../?/?.lua;../share/?.lua;../share/?/?.lua"

local helpers = require('helpers')
local lfs = require('lfs')

local recovery = {}

function recovery.version()
    return "0.0.0"
end

function recovery.branch()
    return "test"
end

function recovery.revision()
    return "BABEF00D"
end

local sys = {}
local bootctrl = {}
local gui = {}

sys.boot_reason_codes = {
    update = 0xF001,
    recovery = 0xF002,
    factory = 0xF003,
    pgm_keys = 0xF004,
    usb_mc_mode = 0xF005,
    backup = 0xF006,
    restore = 0xF007,
    os = 0xF008,
    unknown = 0xF009
}

bootctrl.slot = {
    a = 0,
    b = 1
}

function bootctrl.mark_as_active(slot)
end

function bootctrl.mark_as_bootable(slot)
end

function bootctrl.get_next_active()
end

function sys.sleep(time)
end

function sys.set_os_boot_status()
end

function gui.clear()
end

function gui.display_raw_img(width, height, data)
end

sys.user = stub()
sys.source_slot = stub()
sys.target_slot = stub()
sys.boot_reason = stub()
sys.boot_reason_str = stub()
sys.set_boot_reason = stub()
sys.flash_bootloader = stub()
sys.repartition_fs = stub()
sys.free_space = stub()

recovery.sys = sys
recovery.gui = gui
recovery.bootctrl = bootctrl

package.preload["recovery"] = function()
    return recovery
end

-- Used to recreate the original package.path during switching between products
local ppack = package.path

describe("Factory/backup/restore scripts", function()
    package.path = ppack .. ";../products/PurePhone/?.lua"
    recovery.sys.source_slot.returns("device/system")
    recovery.sys.target_slot.returns("device/target")
    recovery.sys.user.returns("device/user")

    local function invoke_entry()
        require('entry')
    end

    -- Force reload of the 'entry' module after execution of each unit test case
    after_each(function()
        package.loaded['entry'] = false
    end)

    it("invoke factory reset script", function()
        recovery.sys.free_space.returns(1024 * 1024 * 1024)
        recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.factory)
        recovery.sys.boot_reason_str.returns("factory")
        assert.has_no.error(invoke_entry)
    end)
    it("invoke backup script", function()
        recovery.sys.free_space.returns(1024 * 1024 * 1024)
        recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.backup)
        recovery.sys.boot_reason_str.returns("backup")
        assert.has_no.error(invoke_entry)
    end)
    it("invoke backup script, no free space", function()
        recovery.sys.free_space.returns(10)
        recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.backup)
        recovery.sys.boot_reason_str.returns("backup")
        assert.has_no.error(invoke_entry)
    end)
    it("invoke restore script", function()
        recovery.sys.free_space.returns(1024 * 1024 * 1024)
        recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.restore)
        recovery.sys.boot_reason_str.returns("restore")
        assert.has_no.error(invoke_entry)
    end)
    it("invoke restore script, no free space", function()
        recovery.sys.free_space.returns(10)
        recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.restore)
        recovery.sys.boot_reason_str.returns("restore")
        assert.has_no.error(invoke_entry)
    end)
end)

local function remove_test_package(path)
    if helpers.exists(path) then
        helpers.rmdir(path)
    end
end

local function extract_test_package(path, where)
    os.execute(string.format("tar xf %s -C %s", path, where))
end

describe("Update script", function()
    recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.update)
    recovery.sys.boot_reason_str.returns("update")
    recovery.sys.source_slot.returns("update/system")
    recovery.sys.target_slot.returns("update/target")
    recovery.sys.user.returns("update/user")

    package.loaded['paths'] = false
    package.loaded['update'] = false

    -- Prepare test directory and its data
    remove_test_package("update/system")
    remove_test_package("update/target")
    remove_test_package("update/user/temp/update")
    extract_test_package("update/system.tar", "update")
    extract_test_package("update/target.tar", "update")
    extract_test_package("update/update.tar", "update/user/temp")

    it("invoke update script", function()
        local update = require('update')
        assert.has_no.error(update.execute)
    end)
end)

describe("Update script UDM - PurePhone", function()
    package.path = ppack .. ";../products/PurePhone/?.lua"
    recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.update)
    recovery.sys.boot_reason_str.returns("update")
    recovery.sys.source_slot.returns("update_udm/system")
    recovery.sys.target_slot.returns("update_udm/target")
    recovery.sys.user.returns("update_udm/user")

    package.loaded['paths'] = false
    package.loaded['update_udm'] = false
    package.loaded['update_product'] = false

    -- Prepare test directory and its data
    remove_test_package("update_udm/system")
    remove_test_package("update_udm/target")
    remove_test_package("update_udm/user")
    remove_test_package("update_udm/user/temp/update")

    extract_test_package("update_udm/system.tar", "update_udm")
    extract_test_package("update_udm/target.tar", "update_udm")
    extract_test_package("update_udm/user.tar", "update_udm")
    helpers.mkdirp("update_udm/user/temp")
    extract_test_package("update_udm/update.tar", "update_udm/user/temp")

    it("invoke update UDM script", function()
        local update = require('update_udm')
        assert.has_no.error(update.execute)
    end)
end)

describe("Update script UDM - BellHybrid", function()
    package.path = ppack .. ";../products/BellHybrid/?.lua"
    recovery.sys.boot_reason.returns(recovery.sys.boot_reason_codes.update)
    recovery.sys.boot_reason_str.returns("update")
    recovery.sys.source_slot.returns("update_udm/system")
    recovery.sys.target_slot.returns("update_udm/target")
    recovery.sys.user.returns("update_udm/user")

    package.loaded['paths'] = false
    package.loaded['update_udm'] = false
    package.loaded['update_product'] = false

    -- Prepare test directory and its data
    remove_test_package("update_udm/system")
    remove_test_package("update_udm/target")
    remove_test_package("update_udm/user")
    remove_test_package("update_udm/user/temp/update")

    extract_test_package("update_udm/system.tar", "update_udm")
    extract_test_package("update_udm/target.tar", "update_udm")
    extract_test_package("update_udm/user.tar", "update_udm")
    helpers.mkdirp("update_udm/user/temp")
    extract_test_package("update_udm/update.tar", "update_udm/user/temp")

    it("invoke update UDM script", function()
        local update = require('update_udm')
        assert.has_no.error(update.execute)
    end)
end)

