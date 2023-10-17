// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothMessage.hpp"

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}
#include <utility>

BluetoothAddrMessage::BluetoothAddrMessage(Devicei device)
    : sys::DataMessage(MessageType::BluetoothAddrResult), device(device)
{}
BluetoothPairMessage::BluetoothPairMessage(Devicei device)
    : sys::DataMessage(MessageType::BluetoothPairResult), device(device)
{}
auto BluetoothPairMessage::getDevice() const noexcept -> Devicei
{
    return device;
}
