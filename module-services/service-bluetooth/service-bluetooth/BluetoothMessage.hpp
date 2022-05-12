// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Bluetooth/Device.hpp>
#include <Bluetooth/audio/BluetoothAudioDevice.hpp>
#include <Service/Message.hpp>
#include <MessageType.hpp>

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
        Scan,
        StopScan,
        getDevicesAvailable,
        Visible,
        Play,
        Stop,
        Disconnect
    };
    enum Request req = Request::None;
    BluetoothMessage(enum Request req = None) : sys::DataMessage(MessageType::BluetoothRequest), req(req)
    {}
    ~BluetoothMessage() override = default;
};

class BluetoothPairResultMessage : public sys::DataMessage
{
  public:
    explicit BluetoothPairResultMessage(const Devicei &device, bool succeed, std::string errorCode = "")
        : sys::DataMessage(MessageType::BluetoothPairResult), device(device), succeed(succeed),
          errorCode(std::move(errorCode))
    {}
    [[nodiscard]] auto getDevice() const -> Devicei
    {
        return device;
    }
    [[nodiscard]] auto isSucceed() const noexcept -> bool
    {
        return succeed;
    }
    [[nodiscard]] auto getErrorCode() const noexcept -> std::string
    {
        return errorCode;
    }

  private:
    Devicei device;
    bool succeed;
    std::string errorCode;
};

class BluetoothAddrMessage : public sys::DataMessage
{
  public:
    Devicei device;
    explicit BluetoothAddrMessage(Devicei device);
    ~BluetoothAddrMessage() override = default;
};

class BluetoothPairMessage : public sys::DataMessage
{
  public:
    Devicei device;
    explicit BluetoothPairMessage(Devicei device);
    [[nodiscard]] auto getDevice() const noexcept -> Devicei;
    ~BluetoothPairMessage() override = default;
};

class BluetoothAudioStartMessage : public sys::DataMessage
{
  public:
    explicit BluetoothAudioStartMessage(std::shared_ptr<bluetooth::BluetoothAudioDevice> device)
        : DataMessage(MessageType::BluetoothAudioStart), device(std::move(device))
    {}

    auto getAudioDevice() const -> std::shared_ptr<bluetooth::BluetoothAudioDevice>
    {
        return device;
    }

  private:
    std::shared_ptr<bluetooth::BluetoothAudioDevice> device;
};
