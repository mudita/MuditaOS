// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothMessage.hpp"

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}
#include <utility>

BluetoothAddrMessage::BluetoothAddrMessage(std::string addr) : sys::DataMessage(MessageType::BluetoothAddrResult)
{
    sscanf_bd_addr(addr.c_str(), this->addr);
}
BluetoothPairMessage::BluetoothPairMessage(std::string addr)
    : sys::DataMessage(MessageType::BluetoothPairResult), addr(std::move(addr))
{}
