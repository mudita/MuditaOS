// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/SettingsModel.hpp>
#include <common/widgets/list_items/Fraction.hpp>

namespace app::meditation::models
{
    class ChimeInterval : public gui::SettingsModel<list_items::FractionData>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(list_items::FractionData value) override;
        list_items::FractionData getValue() const override;
    };
} // namespace app::meditation::models
