// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Bluetooth/Device.hpp>
#include <list>
#include <vector>
#include <functional>

namespace bluetooth::gap
{
    /// class to store and get devices in GAP operations
    class Devices
    {
        std::list<Devicei> devices;

      public:
        using iter = decltype(devices)::iterator;
        iter find(bd_addr_t &addr);
        iter find(DEVICE_STATE st);
        void for_each(const std::function<void(Devicei &)> &f);
        iter end();
        // adds element, if element already on list - remove and add anew
        iter put(Devicei &&dev);
        // same as above
        iter put(bd_addr_t &addr);
        void clear();
        auto getList() -> std::vector<Devicei>;
    };
} // namespace bluetooth::gap
