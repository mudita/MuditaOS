// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "Device.hpp"
#include "service-bluetooth/BluetoothDevicesModel.hpp"

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

    SECTION("Different addresses and names - != operator")
    {
        device1.setAddress(&addr1);
        device2.setAddress(&addr2);
        REQUIRE(device1 != device3);
    }

    SECTION("Same names and different addresses - != operator")
    {
        device1.setAddress(&addr1);
        device3.setAddress(&addr2);
        REQUIRE(device1 != device3);
    }
}

TEST_CASE("Device handling")
{
    BluetoothDevicesModel devicesModel{nullptr};

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

    devicesModel.insertDevice(device1);
    devicesModel.insertDevice(device2);
    devicesModel.insertDevice(device3);

    SECTION("Add device to the list")
    {
        devicesModel.insertDevice(Devicei{"testDevice"});
        REQUIRE(devicesModel.getDevices().size() == 4);
    }

    SECTION("Merge device list")
    {
        std::vector<Devicei> devicesList, targetList;
        Devicei dummy{"dummyDevice"};
        dummy.setAddress(&addr3);
        devicesList.push_back(dummy);

        targetList.push_back(dummy);
        targetList.push_back(device1);
        targetList.push_back(device2);

        devicesModel.mergeDevicesList(devicesList);

        REQUIRE(!devicesModel.getDeviceByAddress(addr1Str).value().get().name.empty());
        REQUIRE(!devicesModel.getDeviceByAddress(addr2Str).value().get().name.empty());
        REQUIRE(!devicesModel.getDeviceByAddress(addr3Str).value().get().name.empty());
        REQUIRE(devicesModel.getDevices().size() == 3);
    }

    SECTION("Get device with wrong address")
    {
        std::string addrStr{"10:12:6F:E8:9D:05"};
        REQUIRE_FALSE(devicesModel.getDeviceByAddress(addrStr).has_value());
    }

    SECTION("Get device with correct address")
    {
        REQUIRE(strcmp(devicesModel.getDeviceByAddress(addr2Str).value().get().name.data(), "Dev2") == 0);
    }

    SECTION("Remove device from the list")
    {
        devicesModel.removeDevice(device2);
        REQUIRE(devicesModel.getDevices().size() == 2);
    }

    SECTION("Merge devices states - Voice -> Audio")
    {
        device1.deviceState = DeviceState::Paired;

        Devicei updatedDevice = device1;

        updatedDevice.deviceState = DeviceState::ConnectedVoice;
        devicesModel.mergeInternalDeviceState(updatedDevice);
        REQUIRE(devicesModel.getDeviceByAddress(device1.address).value().get().deviceState ==
                DeviceState::ConnectedVoice);

        updatedDevice.deviceState = DeviceState::ConnectedAudio;
        devicesModel.mergeInternalDeviceState(updatedDevice);
        REQUIRE(devicesModel.getDeviceByAddress(device1.address).value().get().deviceState ==
                DeviceState::ConnectedBoth);
    }

    SECTION("Merge devices states - Audio -> Voice")
    {
        device1.deviceState = DeviceState::Paired;

        Devicei updatedDevice = device1;

        updatedDevice.deviceState = DeviceState::ConnectedAudio;
        devicesModel.mergeInternalDeviceState(updatedDevice);
        REQUIRE(devicesModel.getDeviceByAddress(device1.address).value().get().deviceState ==
                DeviceState::ConnectedAudio);

        updatedDevice.deviceState = DeviceState::ConnectedVoice;
        devicesModel.mergeInternalDeviceState(updatedDevice);
        REQUIRE(devicesModel.getDeviceByAddress(device1.address).value().get().deviceState ==
                DeviceState::ConnectedBoth);
    }
}
