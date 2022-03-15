// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/SettingsModel.hpp>

namespace app::meditation::models
{
    class ChimeVolume : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
    };
} // namespace app::meditation::models
