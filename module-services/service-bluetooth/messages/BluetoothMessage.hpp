// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Bluetooth/Device.hpp>
#include <service-bluetooth/ServiceBluetoothCommon.hpp>
#include <Service/Message.hpp>
#include <MessageType.hpp>

#include <utility>
#include <vector>

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
};

class BluetoothMessage : public sys::DataMessage
{
  public:
    enum Request
    {
        None,
        Start,
        Scan,
        StopScan,
        PAN,
        Visible,
        Play,
        Stop
    };
    enum Request req = Request::None;
    BluetoothMessage(enum Request req = None) : sys::DataMessage(MessageType::BluetoothRequest), req(req){};
    ~BluetoothMessage() override = default;
};

class BluetoothScanResultMessage : public sys::DataMessage
{
  public:
    std::vector<Devicei> devices;
    BluetoothScanResultMessage(std::vector<Devicei> devices)
        : sys::DataMessage(MessageType::BluetoothScanResult), devices(std::move(devices)){};
    ~BluetoothScanResultMessage() override = default;
};

class BluetoothPairResultMessage : public sys::DataMessage
{
  public:
    bool status;
    explicit BluetoothPairResultMessage(bool status)
        : sys::DataMessage(MessageType::BluetoothPairResult), status(status){};
};

class BluetoothScanMessage : public sys::DataMessage
{
  public:
    std::vector<Devicei> devices;
    BluetoothScanMessage(std::vector<Devicei> devices)
        : sys::DataMessage(MessageType::BluetoothScanResult), devices(std::move(devices)){};
    ~BluetoothScanMessage() override = default;
};

class BluetoothAddrMessage : public sys::DataMessage
{
  public:
    bd_addr_t addr;
    explicit BluetoothAddrMessage(std::string addr) : sys::DataMessage(MessageType::BluetoothAddrResult)
    {
        sscanf_bd_addr(addr.c_str(), this->addr);
    };
    ~BluetoothAddrMessage() override = default;
};

class BluetoothAudioRegisterMessage : public sys::DataMessage
{
  public:
    QueueHandle_t audioSourceQueue;
    QueueHandle_t audioSinkQueue;
    BluetoothAudioRegisterMessage(QueueHandle_t audioSourceQueue, QueueHandle_t audioSinkQueue)
        : sys::DataMessage(MessageType::BluetoothAudioRegister), audioSourceQueue(audioSourceQueue),
          audioSinkQueue(audioSinkQueue){};
    ~BluetoothAudioRegisterMessage() override = default;
};

class BluetoothDeviceMetadataMessage : public sys::DataMessage
{
  public:
    DeviceMetadata_t metadata;
    BluetoothDeviceMetadataMessage(DeviceMetadata_t metadata)
        : DataMessage(MessageType::BluetoothDeviceMetadata), metadata(std::move(metadata)){};
    ~BluetoothDeviceMetadataMessage() override = default;
};

class BluetoothRequestStreamMessage : public sys::DataMessage
{
  public:
    BluetoothRequestStreamMessage() : DataMessage(MessageType::BluetoothRequestStream){};
    ~BluetoothRequestStreamMessage() override = default;
};

class BluetoothRequestStreamResultMessage : public sys::DataMessage
{
  public:
    BluetoothRequestStreamResultMessage(std::shared_ptr<BluetoothStreamData> data)
        : DataMessage(MessageType::BluetoothRequestStream), data(data){};
    ~BluetoothRequestStreamResultMessage() override = default;

    std::shared_ptr<BluetoothStreamData> getData()
    {
        return data;
    }

  private:
    std::shared_ptr<BluetoothStreamData> data;
};
