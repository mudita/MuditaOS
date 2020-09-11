#pragma once

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
    };
    enum Request req = Request::None;
    BluetoothMessage(enum Request req = None) : sys::DataMessage(MessageType::BluetoothRequest), req(req){};
    ~BluetoothMessage() override = default;
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
    BluetoothAddrMessage(std::string addr) : sys::DataMessage(MessageType::BluetoothAddrResult)
    {
        sscanf_bd_addr(addr.c_str(), this->addr);
    };
    ~BluetoothAddrMessage() override = default;
};
