// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "Device.hpp"
#include "application-settings/models/bluetooth/BluetoothSettingsModel.hpp"

TEST_CASE("Devicei comparison")
{
    Devicei device1{"Dev1"};
    Devicei device2{"Dev2"};
    Devicei device3{"Dev1"};

    bd_addr_t addr1;
    bd_addr_t addr2;

    std::string addr1Str{"00:12:6F:E7:9D:05"};
    std::string addr2Str{"F8:5C:7D:17:E4:8F"};

    sscanf_bd_addr(addr1Str.c_str(), addr1);
    sscanf_bd_addr(addr2Str.c_str(), addr2);

    SECTION("Same addresses and names")
    {
        device1.setAddress(&addr1);
        device3.setAddress(&addr1);
        REQUIRE(device1 == device3);
    }

    SECTION("Different addresses and names")
    {
        device1.setAddress(&addr1);
        device2.setAddress(&addr2);
        REQUIRE_FALSE(device1 == device3);
    }

    SECTION("Same names and different addresses")
    {
        device1.setAddress(&addr1);
        device3.setAddress(&addr2);
        REQUIRE_FALSE(device1 == device3);
    }
}

TEST_CASE("Device handling")
{
    BluetoothSettingsModel settingsModel{nullptr};

    Devicei device1{"Dev1"};
    Devicei device2{"Dev2"};
    Devicei device3{"Dev1"};
    Devicei device4{"Dev4"};
    Devicei dummy{""};

    bd_addr_t addr1;
    bd_addr_t addr2;
    bd_addr_t addr3;

    std::string addr1Str{"00:12:6F:E7:9D:05"};
    std::string addr2Str{"F8:5C:7D:17:E4:8F"};
    std::string addr3Str{"F8:5C:7D:17:E4:00"};

    sscanf_bd_addr(addr1Str.c_str(), addr1);
    sscanf_bd_addr(addr2Str.c_str(), addr2);
    sscanf_bd_addr(addr3Str.c_str(), addr3);

    device1.setAddress(&addr1);
    device2.setAddress(&addr2);
    device3.setAddress(&addr1);
    device4.setAddress(&addr2);

    std::vector<Devicei> devicesList;

    devicesList.push_back(device1);
    devicesList.push_back(device2);
    devicesList.push_back(device3);

    SECTION("Replace devices list")
    {
        settingsModel.replaceDevicesList(devicesList);
        REQUIRE(settingsModel.getDevices().size() == 3);
        REQUIRE(settingsModel.getDevices() == devicesList);
    }

    SECTION("Set active device")
    {
        settingsModel.replaceDevicesList(devicesList);

        settingsModel.setActiveDevice(device2);
        REQUIRE(settingsModel.getActiveDevice().has_value());
        REQUIRE(settingsModel.getActiveDevice().value().get() == device2);
    }

    SECTION("Set wrong active device")
    {
        settingsModel.replaceDevicesList(devicesList);

        settingsModel.setActiveDevice(device4);
        REQUIRE_FALSE(settingsModel.getActiveDevice().has_value());
    }

    SECTION("Set selected device")
    {
        settingsModel.replaceDevicesList(devicesList);

        settingsModel.setSelectedDevice(device2);
        REQUIRE(settingsModel.getSelectedDevice().has_value());
        REQUIRE(settingsModel.getSelectedDevice().value().get() == device2);
    }

    SECTION("Set wrong selected device")
    {
        settingsModel.replaceDevicesList(devicesList);

        settingsModel.setSelectedDevice(device4);
        REQUIRE_FALSE(settingsModel.getSelectedDevice().has_value());
    }

    SECTION("Set active device state")
    {
        settingsModel.replaceDevicesList(devicesList);

        settingsModel.setActiveDevice(device2);
        settingsModel.setActiveDeviceState(DeviceState::ConnectedBoth);
        REQUIRE(settingsModel.getActiveDevice().value().get().deviceState == DeviceState::ConnectedBoth);
    }

    SECTION("Is device connecting? - true")
    {
        settingsModel.replaceDevicesList(devicesList);

        settingsModel.setActiveDevice(device2);
        settingsModel.setActiveDeviceState(DeviceState::Connecting);
        REQUIRE(settingsModel.isDeviceConnecting());
    }

    SECTION("Is device connecting? - false")
    {
        settingsModel.replaceDevicesList(devicesList);

        settingsModel.setActiveDevice(device2);
        settingsModel.setActiveDeviceState(DeviceState::ConnectedBoth);
        REQUIRE_FALSE(settingsModel.isDeviceConnecting());
    }
}
