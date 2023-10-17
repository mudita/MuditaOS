// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <models/SettingsModel.hpp>
#include <utf8/UTF8.hpp>
#include <widgets/list_items/Fraction.hpp>
namespace gui
{
    template <class ValueType>
    SettingsModel<ValueType>::SettingsModel(sys::Service *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    template class SettingsModel<bool>;
    template class SettingsModel<std::uint8_t>;
    template class SettingsModel<std::uint32_t>;
    template class SettingsModel<std::string>;
    template class SettingsModel<UTF8>;
    template class SettingsModel<time_t>;
    template class SettingsModel<app::list_items::FractionData>;
} // namespace gui
