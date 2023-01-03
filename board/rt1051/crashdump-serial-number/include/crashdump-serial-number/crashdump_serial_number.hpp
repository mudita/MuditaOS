// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// The purpose of this file is to keep the devices serial number in the memory.
// In case of any crash, the serial number could be easily added to a crash dump filename.
// It will allow to identify the crash even after it is moved or copied from the device.
// setSerialNumber() should be called during system initialization.
// getSerialNumber() is called by the crash dump writer during file creation.

#pragma once
#include <string>
namespace crashdump
{
    void setSerialNumber(const std::string &sn);
    std::string getSerialNumber();

} // namespace crashdump
