// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <serial-number-parser/SerialNumberParser.hpp>
#include <Utils.hpp>
#include <cstring>
#include <map>

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
    } // namespace

    bool isOldSerialNumberFormat(const std::string &serialNumber)
    {
        return (serialNumber.find_first_not_of("0123456789") == std::string::npos);
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

            return VersionMetadata(item->second, secondVersion);
        }
    }
} // namespace serial_number_parser
