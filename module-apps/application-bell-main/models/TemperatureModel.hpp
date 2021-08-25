// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Temperature.hpp>

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
        std::function<void(gui::temperature::Temperature)> onTemperatureUpdate;

        virtual ~AbstractTemperatureModel() noexcept = default;

        virtual gui::temperature::Temperature getTemperature() const = 0;
    };

    class TemperatureModel : public AbstractTemperatureModel
    {
      public:
        explicit TemperatureModel(app::Application *app);
        gui::temperature::Temperature getTemperature() const override;
    };
} // namespace app::home_screen
