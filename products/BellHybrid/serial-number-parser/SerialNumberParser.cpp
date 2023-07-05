// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <serial-number-parser/SerialNumberParser.hpp>
#include <purefs/vfs_subsystem.hpp>
#include <Utils.hpp>
#include <cstring>

/*
 * Old SN is in MMYYWWNNNNNNN form, where:
 * MM - model code, 04 for Harmony;
 * YY - production year;
 * WW - production week;
 * NNNNNNN - unique device number, defined per batch.
 *
 * ID string is a substring consisting of first 6 SN digits.
 *
 *
 * New SN is in MMRRCYYWWNNNN form, where:
 * MM - model code, 04 for Harmony;
 * RR - hardware revision, P1, P2, P3...;
 * C - case colour; G = gray, B = black;
 * YY - production year;
 * WW - production week;
 * NNNN - unique device number, defined per batch.
 *
 */

namespace serial_number_parser
{
    namespace
    {
        /* Block device constants */
        constexpr auto blkdevName  = "emmc0sys1";
        constexpr auto blockSize   = 512;
        constexpr auto blockToRead = 2;
        constexpr auto blocksCount = 1;

        /* Common constants */
        constexpr auto serialNumberOffset  = 0;
        constexpr auto serialNumberLength  = 13;
        constexpr auto modelCodeOffset     = 0;
        constexpr auto modelCodeLength     = 2;
        constexpr auto unknownSerialNumber = "0000000000000";

        constexpr auto grayColor           = "gray";
        constexpr auto blackColor          = "black";
        constexpr auto unknownColour       = "unknown";

        constexpr auto firstVersion   = 1;
        constexpr auto secondVersion  = 2;
        constexpr auto unknownVersion = 0;

        constexpr auto firstModelCode = "04";

        const auto unknownVersionMetadata = VersionMetadata(unknownColour, unknownVersion);
        const auto unknownDeviceMetadata  = DeviceMetadata(unknownSerialNumber, unknownVersionMetadata);

        /* Old serial number constants */
        constexpr auto idStringOffset = 0;
        constexpr auto idStringLength = 6;

        /* ID string to version metadata map for already released batches */
        const std::map<std::string, VersionMetadata> idStringToVersionInfoMap{
            {"042148", VersionMetadata(grayColor, firstVersion)},
            {"042213", VersionMetadata(grayColor, firstVersion)},
            {"042242", VersionMetadata(blackColor, secondVersion)}};

        /* New serial number constants */
        constexpr auto colourCodeOffset = 4;
        const std::map<char, std::string> colourCodeToColourMap{{'G', grayColor}, {'B', blackColor}};

        bool isOldSerialNumberFormat(const std::string &serialNumber)
        {
            return (serialNumber.find_first_not_of("0123456789") == std::string::npos);
        }

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

        std::optional<VersionMetadata> getDeviceVersionMetadata(const std::string &serialNumber)
        {
            if (isOldSerialNumberFormat(serialNumber)) {
                LOG_INFO("Device has old serial number format");
                const auto idString = serialNumber.substr(idStringOffset, idStringLength);
                const auto item     = idStringToVersionInfoMap.find(idString);
                if (item == idStringToVersionInfoMap.end()) {
                    return std::nullopt;
                }
                return item->second;
            }
            else {
                LOG_INFO("Device has new serial number format");
                const auto colourCode = serialNumber[colourCodeOffset];
                const auto item       = colourCodeToColourMap.find(colourCode);
                if (item == colourCodeToColourMap.end()) {
                    return std::nullopt;
                }

                /* TODO
                 * Verify this algorithm. It will work when next Harmony versions will have
                 * next model codes starting from "04", i.e. Harmony 2 is "05", Harmony 3 is "06",-u etc.
                 * Maybe it should be determined using hardware version? (see SN description at the
                 * beginning of this file) */
                const auto modelCode     = serialNumber.substr(modelCodeOffset, modelCodeLength);
                const auto versionNumber = utils::toNumeric(modelCode) - utils::toNumeric(firstModelCode) + 1;

                return VersionMetadata(item->second, versionNumber);
            }
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
