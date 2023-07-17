// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <serial-number-parser/DeviceMetadata.hpp>
#include <serial-number-parser/SerialNumberParser.hpp>
#include <purefs/vfs_subsystem.hpp>
#include <log/log.hpp>

namespace serial_number_parser
{
    namespace
    {
        /* Block device constants */
        constexpr auto blkdevName  = "emmc0sys1";
        constexpr auto blockSize   = 512;
        constexpr auto blockToRead = 2;
        constexpr auto blocksCount = 1;

        constexpr auto serialNumberOffset = 0;
        constexpr auto serialNumberLength = 13;

        const auto unknownVersionMetadata = VersionMetadata(unknownColour, unknownVersion);
        const auto unknownDeviceMetadata  = DeviceMetadata(unknownSerialNumber, unknownVersionMetadata);

        std::string readSerialNumber()
        {
            char block[blockSize];

            const auto diskManager = purefs::subsystem::disk_mgr();
            if (diskManager == nullptr) {
                return "";
            }

            const auto status = diskManager->read(blkdevName, block, blockToRead, blocksCount);
            if (status != 0) {
                return "";
            }

            return std::string(&block[serialNumberOffset], serialNumberLength);
        }
    } // namespace

    DeviceMetadata getDeviceMetadata()
    {
        const auto serialNumber = readSerialNumber();
        if (serialNumber.empty()) {
            LOG_ERROR("Error reading serial number from eMMC!");
            return unknownDeviceMetadata;
        }

        const auto deviceVersionMetadata = getDeviceVersionMetadata(serialNumber);
        if (!deviceVersionMetadata.has_value()) {
            LOG_ERROR("Failed to read device version metadata for device with serial number '%s'!",
                      serialNumber.c_str());
            return DeviceMetadata(serialNumber, unknownVersionMetadata);
        }

        return DeviceMetadata(serialNumber, deviceVersionMetadata.value());
    }
} // namespace serial_number_parser
