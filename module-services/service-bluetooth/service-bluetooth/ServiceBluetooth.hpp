// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Bluetooth/BluetoothWorker.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include "service-bluetooth/SettingsHolder.hpp"
#include "BluetoothMessage.hpp"

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
    void sendWorkerCommand(bluetooth::Command command);
    QueueHandle_t workerQueue = nullptr;
    std::shared_ptr<bluetooth::SettingsHolder> settingsHolder;

  private:
    std::unique_ptr<BluetoothWorker> worker;
};
