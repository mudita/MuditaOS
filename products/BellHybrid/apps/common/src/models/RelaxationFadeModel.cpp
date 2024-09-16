// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "models/RelaxationFadeModel.hpp"
#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>

namespace app
{
    auto RelaxationFadeSettings::setValue(bool value) -> void
    {
        const auto valStr = std::to_string(value);
        settings.setValue(bell::settings::Relaxation::fadeActive, valStr, settings::SettingsScope::Global);
    }

    auto RelaxationFadeSettings::getValue() const -> bool
    {
        const auto str = settings.getValue(bell::settings::Relaxation::fadeActive, settings::SettingsScope::Global);
        return utils::toNumeric(str);
    }
} // namespace app
