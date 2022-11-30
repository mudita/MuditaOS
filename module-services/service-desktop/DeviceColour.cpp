// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-desktop/DeviceColour.hpp>
#include <map>
#include <cstdio>
namespace
{
    constexpr auto NONE = "none";
    const std::map<std::string, std::uint16_t> colour_to_version_map{
        {NONE, 0x0111}, {"black", 0x0112}, {"gray", 0x0113}};

} // namespace

namespace device_colour
{
    std::uint16_t getColourVersion(const std::string &colour)
    {
        auto it = colour_to_version_map.find(colour);
        if (it == colour_to_version_map.end()) {
            return colour_to_version_map.at(NONE);
        }
        return colour_to_version_map.at(colour);
    }
} // namespace device_colour
