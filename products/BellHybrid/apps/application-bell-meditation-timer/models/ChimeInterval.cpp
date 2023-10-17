// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChimeInterval.hpp"
#include <Utils.hpp>

namespace
{
    constexpr auto chime_interval_db = "chime_interval";

    /// Fraction numbers are stored using "n/d" format.
    app::list_items::FractionData to_fraction_data(const std::string &str)
    {
        const std::string delimiter = "/";
        std::string nominator       = str.substr(0, str.find(delimiter));
        std::string denominator     = str.substr(str.find(delimiter) + 1);
        return {utils::getNumericValue<int>(nominator), utils::getNumericValue<int>(denominator)};
    }
    std::string from_fraction_data(const app::list_items::FractionData &data)
    {
        return std::to_string(data.nominator) + "/" + std::to_string(data.denominator);
    }
} // namespace

namespace app::meditation::models
{
    void ChimeInterval::setValue(app::list_items::FractionData value)
    {
        settings.setValue(chime_interval_db, from_fraction_data(value));
    }
    app::list_items::FractionData ChimeInterval::getValue() const
    {
        const auto value = settings.getValue(chime_interval_db);
        if (value.empty()) {
            /// If not found, return 1/1 which corresponds to 'OFF'
            return {1, 1};
        }
        return to_fraction_data(value);
    }
} // namespace app::meditation::models
