// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace app
{
    class Application;
}

namespace app::home_screen
{
    class AbstractTemperatureModel
    {
      public:
        struct Temperature
        {
            enum class Unit
            {
                Celsius,
                Fahrenheit
            };
            Unit unit;
            std::int32_t value;
        };

        std::function<void(Temperature)> onTemperatureUpdate;

        virtual ~AbstractTemperatureModel() noexcept = default;

        virtual Temperature getTemperature() const = 0;

        static std::string unitToStr(Temperature::Unit unit);
    };

    class TemperatureModel : public AbstractTemperatureModel
    {
      public:
        explicit TemperatureModel(app::Application *app);
        Temperature getTemperature() const override;
    };
} // namespace app::home_screen
