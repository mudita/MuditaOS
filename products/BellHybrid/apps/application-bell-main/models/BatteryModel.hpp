// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-utils/EventStore/EventStore.hpp>
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
    class AbstractBatteryModel
    {
      public:
        virtual ~AbstractBatteryModel() noexcept = default;

        virtual Store::Battery getLevelState() const = 0;
    };

    class BatteryModel : public AbstractBatteryModel
    {
      public:
        explicit BatteryModel(app::ApplicationCommon *app);
        Store::Battery getLevelState() const;

      private:
        mutable settings::Settings settings;
    };
} // namespace app::home_screen
