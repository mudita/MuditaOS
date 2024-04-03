// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/SettingsModel.hpp>

namespace app::clock::models
{
    class DeepRefreshInterval : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;

        std::uint8_t refreshInterval{0U};
    };
} // namespace app::clock::models
