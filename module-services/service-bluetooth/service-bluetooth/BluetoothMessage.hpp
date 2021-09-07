// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceBluetoothCommon.hpp"

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
        Start,
        Scan,
        StopScan,
        getDevicesAvailable,
        PAN,
        Visible,
        Play,
        SwitchProfile,
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
    explicit BluetoothPairResultMessage(std::string addr, bool succeed)
        : sys::DataMessage(MessageType::BluetoothPairResult), addr(std::move(addr)), succeed(succeed)
    {}
    [[nodiscard]] auto getAddr() const -> std::string
    {
        return addr;
    }
    [[nodiscard]] auto isSucceed() const noexcept -> bool
    {
        return succeed;
    }

  private:
    std::string addr;
    std::string name;
    bool succeed;
};

class BluetoothAddrMessage : public sys::DataMessage
{
  public:
    bd_addr_t addr;
    explicit BluetoothAddrMessage(std::string addr);
    ~BluetoothAddrMessage() override = default;
};

class BluetoothPairMessage : public sys::DataMessage
{
  public:
    std::string addr;
    explicit BluetoothPairMessage(std::string addr);
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
