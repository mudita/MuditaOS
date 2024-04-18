// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusSettingsModel.hpp"
#include <Utils.hpp>

namespace app::focus::models
{
    FocusSettingsModel::FocusSettingsModel(sys::Service *app, const std::string &name, std::uint8_t defaultValue)
        : SettingsModel{app}, name{name}, defaultValue{defaultValue}
    {}

    void FocusSettingsModel::setValue(std::uint8_t value)
    {
        settings.setValue(name, std::to_string(value));
    }

    std::uint8_t FocusSettingsModel::getValue() const
    {
        const auto &value = settings.getValue(name);
        if (value.empty()) {
            return defaultValue;
        }
        return utils::getNumericValue<std::uint8_t>(value);
    }
} // namespace app::focus::models
