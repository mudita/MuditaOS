// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceBluetoothCommon.hpp"

#include <Bluetooth/Device.hpp>
#include <Service/Message.hpp>
#include <Audio/Stream.hpp>
#include <MessageType.hpp>
#include <bsp_audio.hpp>

#include <utility>
#include <vector>

struct BluetoothStatus
{
    enum class State
    {
        Off,
        On,
        Error,
        None
    } state;
    bool visibility;
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
        StopPlayback
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
    explicit BluetoothAddrMessage(std::string addr);
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

class BluetoothProxyMessage : public sys::DataMessage
{
  public:
    BluetoothProxyMessage(MessageType messageType, bsp::AudioDevice::Format format)
        : DataMessage(messageType), format(format){};

    ~BluetoothProxyMessage() override = default;

    bsp::AudioDevice::Format format;
};

/// Bluetooth proxy messages

class BluetoothProxyStartMessage : public BluetoothProxyMessage
{
  public:
    BluetoothProxyStartMessage(audio::Stream &streamOut, audio::Stream &streamIn, bsp::AudioDevice::Format format)
        : BluetoothProxyMessage(MessageType::BluetoothProxyStart, format), audioStreamOut(streamOut),
          audioStreamIn(streamIn){};

    ~BluetoothProxyStartMessage() override = default;

    audio::Stream &audioStreamOut;
    audio::Stream &audioStreamIn;
};

class BluetoothProxyStopMessage : public BluetoothProxyMessage
{
  public:
    BluetoothProxyStopMessage(bsp::AudioDevice::Format format)
        : BluetoothProxyMessage(MessageType::BluetoothProxyStop, format){};
    ~BluetoothProxyStopMessage() override = default;
};

class BluetoothProxySetVolumeMessage : public BluetoothProxyMessage
{
  public:
    BluetoothProxySetVolumeMessage(bsp::AudioDevice::Format format)
        : BluetoothProxyMessage(MessageType::BluetoothProxyOutputVolumeCtrl, format){};
    ~BluetoothProxySetVolumeMessage() override = default;
};

class BluetoothProxySetGainMessage : public BluetoothProxyMessage
{
  public:
    BluetoothProxySetGainMessage(bsp::AudioDevice::Format format)
        : BluetoothProxyMessage(MessageType::BluetoothProxyInputGainCtrl, format){};
    ~BluetoothProxySetGainMessage() override = default;

    float value;
};

class BluetoothProxySetOutputPathMessage : public BluetoothProxyMessage
{
  public:
    BluetoothProxySetOutputPathMessage(bsp::AudioDevice::Format format)
        : BluetoothProxyMessage(MessageType::BluetoothProxyOutputPathCtrl, format){};
    ~BluetoothProxySetOutputPathMessage() override = default;
};

class BluetoothProxySetInputPathMessage : public BluetoothProxyMessage
{
  public:
    BluetoothProxySetInputPathMessage(bsp::AudioDevice::Format format)
        : BluetoothProxyMessage(MessageType::BluetoothProxyInputPathCtrl, format){};
    ~BluetoothProxySetInputPathMessage() override = default;
};
