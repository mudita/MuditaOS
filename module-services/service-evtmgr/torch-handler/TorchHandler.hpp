// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <type_traits>

namespace torch
{
    /// direct mapping to ColourTemperature
    /// it can be exported in some shared header if it has to

    typedef float Intensity;

    struct TorchConfig
    {
        Intensity inYellow;
        Intensity inWhite;
    };

    static_assert(std::is_pod<TorchConfig>(), "This is only simple data carrier");

    class TorchHandler
    {
        struct TorchConfig config
        {
            1, 1
        };
        unsigned short intensityIn_mA(Intensity val);
        void setIntensityByColor();

      public:
        void init(TorchConfig config);
        void toggleOnOff();
        void toggleColor();
        void off();
    };

} // namespace torch
