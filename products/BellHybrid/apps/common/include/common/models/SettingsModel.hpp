// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>

#include <service-db/Settings.hpp>
#include <Service/Service.hpp>

namespace app
{
    class ApplicationCommon;
};

namespace gui
{
    template <class ValueType> class SettingsModel : public AbstractSettingsModel<ValueType>
    {
      public:
        explicit SettingsModel(sys::Service *app);

      protected:
        mutable settings::Settings settings;
    };
} // namespace gui
