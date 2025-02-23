// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/SettingsModel.hpp>
#include <chrono>

namespace app::bell_settings
{
    class AbstractBedsideModel
    {
      public:
        AbstractBedsideModel(std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> &&brightnessModel,
                             std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> &&timeModel)
            : brightnessModel{std::move(brightnessModel)}, timeModel{std::move(timeModel)}
        {}

        virtual ~AbstractBedsideModel() = default;

        auto getBrightnessModel() -> gui::AbstractSettingsModel<std::uint8_t> &
        {
            return *brightnessModel;
        }

        auto getTimeModel() -> gui::AbstractSettingsModel<std::uint8_t> &
        {
            return *timeModel;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> brightnessModel;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> timeModel;
    };

    class BedsideBrightnessModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(std::uint8_t value) -> void override;
        auto getValue() const -> std::uint8_t override;
    };

    class BedsideTimeModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(std::uint8_t value) -> void override;
        auto getValue() const -> std::uint8_t override;
    };

    class BedsideModel : public AbstractBedsideModel
    {
      public:
        BedsideModel(std::unique_ptr<BedsideBrightnessModel> &&brightnessModel,
                     std::unique_ptr<BedsideTimeModel> &&timeModel)
            : AbstractBedsideModel{std::move(brightnessModel), std::move(timeModel)}
        {}
    };
} // namespace app::bell_settings
