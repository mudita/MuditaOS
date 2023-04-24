// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <crashdump-serial-number/crashdump_serial_number.hpp>

namespace crashdump
{
    namespace
    {
        std::string serialNumber{};
    }

    void setSerialNumber(const std::string &sn)
    {
        serialNumber = sn;
    }

    const std::string &getSerialNumber()
    {
        return serialNumber;
    }

} // namespace crashdump
