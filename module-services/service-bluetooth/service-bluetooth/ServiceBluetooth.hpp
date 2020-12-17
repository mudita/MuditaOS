// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BluetoothMessage.hpp"
#include <Bluetooth/BluetoothWorker.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include <memory> // for unique_ptr

class BluetoothWorker;
namespace settings
{
    class Settings;
}

class ServiceBluetooth : public sys::Service
{

  public:
    ServiceBluetooth();
    ~ServiceBluetooth();

    virtual sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    virtual sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

  private:
    std::unique_ptr<BluetoothWorker> worker;
    std::unique_ptr<settings::Settings> settingsProvider;

    // will be replaced with settings storage introduced in [EGD-4579]
    BluetoothStatus btStatus;
    std::vector<Devicei> devices{Devicei("Paired_device1"),
                                 Devicei("Paired_device2"),
                                 Devicei("Paired_device3"),
                                 Devicei("Paired_device4"),
                                 Devicei("Paired_device5")};
    std::string phoneName = "PurePhone";

    void stateSettingChanged(std::string value);
    void deviceVisibilitySettingChanged(std::string value);
    void deviceNameSettingChanged(std::string value);
    void bondedDevicesSettingChanged(std::string value);
};
