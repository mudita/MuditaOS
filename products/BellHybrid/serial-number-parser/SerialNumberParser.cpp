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
        constexpr auto unknownSerialNumber = "0000000000000";
        constexpr auto unknownColour       = "unknown";

        /* Old serial number constants */
        constexpr auto idStringOffset = 0;
        constexpr auto idStringLength = 6;

        /* ID string to colour map for already released batches */
        const std::map<std::string, std::string> idStringToColourMap{
            {"042148", "gray"}, {"042213", "gray"}, {"042242", "black"}};

        /* New serial number constants */
        constexpr auto colourCodeOffset = 4;
        const std::map<char, std::string> colourCodeToColourMap{{'G', "gray"}, {'B', "black"}};

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

        std::string getDeviceColour(const std::string &serialNumber)
        {
            if (isOldSerialNumberFormat(serialNumber)) {
                LOG_INFO("Device has old serial number format");
                const auto idString = serialNumber.substr(idStringOffset, idStringLength);
                const auto item     = idStringToColourMap.find(idString);
                if (item == idStringToColourMap.end()) {
                    return "";
                }
                return item->second;
            }
            else {
                LOG_INFO("Device has new serial number format");
                const auto colourCode = serialNumber[colourCodeOffset];
                const auto item       = colourCodeToColourMap.find(colourCode);
                if (item == colourCodeToColourMap.end()) {
                    return "";
                }
                return item->second;
            }
        }
    } // namespace

    std::pair<std::string, std::string> getDeviceMetadata()
    {
        const auto serialNumber = readSerialNumber();
        if (serialNumber.empty()) {
            LOG_ERROR("Error reading serial number from eMMC!");
            return {unknownSerialNumber, unknownColour};
        }

        const auto deviceColour = getDeviceColour(serialNumber);
        if (deviceColour.empty()) {
            LOG_ERROR("Failed to read colour for device with serial number '%s'!", serialNumber.c_str());
            return {serialNumber, unknownColour};
        }

        return {serialNumber, deviceColour};
    }
} // namespace serial_number_parser
