// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/Temperature.hpp>
#include <service-db/Settings.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace app
{
    class ApplicationCommon;
}

namespace app::home_screen
{
    class AbstractTemperatureModel
    {
      public:
        virtual ~AbstractTemperatureModel() noexcept = default;

        virtual utils::temperature::Temperature getTemperature() const = 0;
    };

    class TemperatureModel : public AbstractTemperatureModel
    {
      public:
        explicit TemperatureModel(app::ApplicationCommon *app);
        utils::temperature::Temperature getTemperature() const override;

      private:
        mutable settings::Settings settings;
    };
} // namespace app::home_screen
