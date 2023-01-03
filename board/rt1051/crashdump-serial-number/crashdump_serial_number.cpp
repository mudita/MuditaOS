// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <crashdump-serial-number/crashdump_serial_number.hpp>

namespace
{
    std::string serial_number = "0000000000000";
}
namespace crashdump
{
    void setSerialNumber(const std::string &sn)
    {
        serial_number = sn;
    }
    std::string getSerialNumber()
    {
        return serial_number;
    }

} // namespace crashdump
