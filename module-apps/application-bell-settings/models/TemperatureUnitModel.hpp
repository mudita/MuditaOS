// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Temperature.hpp>
#include <service-db/Settings.hpp>

#include <memory>

namespace app
{
    class Application;
}

namespace app::bell_settings
{
    class AbstractTemperatureUnitModel
    {
      public:
        virtual ~AbstractTemperatureUnitModel() = default;

        virtual gui::temperature::Temperature::Unit get() const = 0;

        virtual void set(gui::temperature::Temperature::Unit unit) = 0;
    };

    class TemperatureUnitModel : public AbstractTemperatureUnitModel
    {
      public:
        explicit TemperatureUnitModel(Application *app);

        gui::temperature::Temperature::Unit get() const override;

        void set(gui::temperature::Temperature::Unit unit) override;

      private:
        mutable settings::Settings settings;
    };
} // namespace app::bell_settings
