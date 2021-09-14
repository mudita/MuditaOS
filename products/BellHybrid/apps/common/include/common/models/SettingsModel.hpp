// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractSettingsModel.hpp"

#include <service-db/Settings.hpp>

namespace app
{
    class ApplicationCommon;
};

namespace gui
{
    template <class ValueType> class SettingsModel : public AbstractSettingsModel<ValueType>
    {
      public:
        explicit SettingsModel(app::ApplicationCommon *app);

      protected:
        mutable settings::Settings settings;
    };
} // namespace gui
