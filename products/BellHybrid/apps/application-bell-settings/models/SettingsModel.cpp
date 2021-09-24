// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <models/SettingsModel.hpp>

#include <apps-common/ApplicationCommon.hpp>

namespace gui
{
    template <class ValueType> SettingsModel<ValueType>::SettingsModel(app::ApplicationCommon *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    template class SettingsModel<bool>;
    template class SettingsModel<std::uint8_t>;
    template class SettingsModel<UTF8>;
} // namespace gui
