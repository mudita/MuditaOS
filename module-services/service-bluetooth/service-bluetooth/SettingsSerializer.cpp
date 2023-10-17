// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsSerializer.hpp"
#include "btstack_util.h"
#include <log/log.hpp>

namespace strings
{
    constexpr inline auto addr    = "addr";
    constexpr inline auto name    = "name";
    constexpr inline auto devices = "devices";
} // namespace strings

auto SettingsSerializer::toString(const std::vector<Devicei> &devices) -> std::string
{
    json11::Json::array devicesJson;
    for (auto &device : devices) {
        auto deviceEntry = json11::Json::object{{strings::addr, bd_addr_to_str(device.address)},
                                                {strings::name, std::string{device.name.data()}}};
        devicesJson.emplace_back(deviceEntry);
    }
    json11::Json finalJson = json11::Json::object{{strings::devices, devicesJson}};

    return finalJson.dump();
}
auto SettingsSerializer::fromString(const std::string &jsonStr) -> std::vector<Devicei>
{
    json11::Json devicesJson;
    std::string err;
    devicesJson = json11::Json::parse(jsonStr.c_str(), err);
    if (!err.empty()) {
        LOG_ERROR("Failed parsing device string!");
        return std::vector<Devicei>();
    }
    json11::Json::array devicesArray;
    devicesArray = std::move(devicesJson[strings::devices].array_items());

    std::vector<Devicei> devicesVector;
    for (auto &device : devicesArray) {
        Devicei temp;
        sscanf_bd_addr(device[strings::addr].string_value().c_str(), temp.address);
        strcpy(temp.name.data(), device[strings::name].string_value().c_str());
        temp.deviceState = DeviceState::Paired;
        devicesVector.emplace_back(temp);
    }
    return devicesVector;
}
