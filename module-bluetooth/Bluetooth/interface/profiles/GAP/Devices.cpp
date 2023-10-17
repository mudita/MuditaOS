// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Devices.hpp"
#include <btstack_util.h>

namespace bluetooth::gap
{
    auto Devices::getList() -> std::vector<Devicei>
    {
        return {devices.begin(), devices.end()};
    }

    Devices::iter Devices::find(bd_addr_t &addr)
    {
        return std::find_if(std::begin(devices), std::end(devices), [addr](const Devicei &device) {
            return bd_addr_cmp(addr, device.address) == 0;
        });
    }

    Devices::iter Devices::find(DEVICE_STATE st)
    {
        return std::find_if(
            std::begin(devices), std::end(devices), [st](const Devicei &device) { return st == device.state; });
    }

    void Devices::for_each(const std::function<void(Devicei &)> &f)
    {
        if (not f) {
            return;
        }
        for (auto &el : devices) {
            f(el);
        }
    }

    Devices::iter Devices::end()
    {
        return std::end(devices);
    }

    Devices::iter Devices::put(Devicei &&dev)
    {
        if (const auto &it = find(dev.address); it != end()) {
            devices.erase(it);
        }
        devices.emplace_back(dev);
        return std::prev(devices.end());
    }

    Devices::iter Devices::put(bd_addr_t &addr)
    {
        if (const auto &it = find(addr); it != end()) {
            devices.erase(it);
        }
        devices.emplace_back(Devicei(addr));
        return std::prev(devices.end());
    }

    void Devices::clear()
    {
        devices.clear();
    }
} // namespace bluetooth::gap
