#pragma once

#include "Bluetooth/Device.hpp"
#include "MessageType.hpp"
#include "Service/Message.hpp"

#include <utility>
#include "MessageType.hpp"
#include "Bluetooth/Device.hpp"

extern "C"
{
#include "module-bluetooth/lib/btstack/src/btstack_util.h"
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
