// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/SettingsModel.hpp>

namespace app::focus::models
{
    class FocusSettingsModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        FocusSettingsModel(sys::Service *app, const std::string &name, std::uint8_t defaultValue);

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;

      private:
        const std::string name;
        const std::uint8_t defaultValue;
    };
} // namespace app::focus::models
