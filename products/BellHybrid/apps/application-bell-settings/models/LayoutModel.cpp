// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/models/LayoutModel.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{

    LayoutModel::LayoutModel(ApplicationCommon *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    std::optional<std::string> LayoutModel::getValue() const
    {
        if (const auto result = settings.getValue(bell::settings::Layout::layout, settings::SettingsScope::Global);
            not result.empty()) {
            return result;
        }

        return std::nullopt;
    }

    void LayoutModel::setValue(const std::string &value) const
    {
        settings.setValue(bell::settings::Layout::layout, value, settings::SettingsScope::Global);
    }
} // namespace app::bell_settings