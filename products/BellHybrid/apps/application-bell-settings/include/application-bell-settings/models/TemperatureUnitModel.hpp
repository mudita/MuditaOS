// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/Temperature.hpp>
#include <service-db/Settings.hpp>

#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::bell_settings
{
    class AbstractTemperatureUnitModel
    {
      public:
        virtual ~AbstractTemperatureUnitModel() = default;

        virtual utils::temperature::Temperature::Unit get() const = 0;

        virtual void set(utils::temperature::Temperature::Unit unit) = 0;
    };

    class TemperatureUnitModel : public AbstractTemperatureUnitModel
    {
      public:
        explicit TemperatureUnitModel(ApplicationCommon *app);

        utils::temperature::Temperature::Unit get() const override;

        void set(utils::temperature::Temperature::Unit unit) override;

      private:
        mutable settings::Settings settings;
    };
} // namespace app::bell_settings
