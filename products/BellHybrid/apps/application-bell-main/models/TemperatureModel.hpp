// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Temperature.hpp>
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

        [[nodiscard]] virtual utils::temperature::Temperature getTemperature() const = 0;
    };

    class TemperatureModel : public AbstractTemperatureModel
    {
      public:
        explicit TemperatureModel(app::ApplicationCommon *app);
        [[nodiscard]] utils::temperature::Temperature getTemperature() const override;

      private:
        mutable settings::Settings settings;
    };
} // namespace app::home_screen
