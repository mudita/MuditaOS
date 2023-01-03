// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/vfs_subsystem.hpp>

#include <serial-number-reader/SerialNumberReader.hpp>

namespace
{
    constexpr auto DEVICE_NAME      = "emmc0sys1";
    constexpr auto EMMC_SN_LENGTH   = 13;
    constexpr auto EMMC_BUFFER_SIZE = 512;
    constexpr auto SECTOR_TO_READ   = 2;
    constexpr auto SECTOR_COUNT     = 1U;

    std::string readSerialNumberFromEmmc()
    {
        char buffer[EMMC_BUFFER_SIZE];
        purefs::subsystem::disk_mgr()->read(DEVICE_NAME, buffer, SECTOR_TO_READ, SECTOR_COUNT);
        buffer[EMMC_SN_LENGTH] = '\0';
        return std::string(buffer);
    }
} // namespace
namespace serial_number_reader
{
    std::string readSerialNumber()
    {
        return readSerialNumberFromEmmc();
    }
} // namespace serial_number_reader
