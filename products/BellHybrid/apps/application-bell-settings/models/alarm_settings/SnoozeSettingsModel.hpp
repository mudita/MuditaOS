// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractSnoozeSettingsModel.hpp"
#include <models/SettingsModel.hpp>

namespace app::bell_settings
{
    class SnoozeOnOffModel : public gui::SettingsModel<bool>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(bool value) override;
        bool getValue() const override;
    };

    class SnoozeLengthModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
    };

    class SnoozeChimeIntervalModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
    };

    class SnoozeChimeToneModel : public gui::SettingsModel<UTF8>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(UTF8 value) override;
        UTF8 getValue() const override;
    };

    class SnoozeChimeVolumeModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
    };

    class SnoozeSettingsModel : public AbstractSnoozeSettingsModel
    {
      public:
        SnoozeSettingsModel(std::unique_ptr<SnoozeOnOffModel> snoozeOnOffModel,
                            std::unique_ptr<SnoozeLengthModel> snoozeLengthModel,
                            std::unique_ptr<SnoozeChimeIntervalModel> snoozeChimeIntervalModel,
                            std::unique_ptr<SnoozeChimeToneModel> snoozeChimeToneModel,
                            std::unique_ptr<SnoozeChimeVolumeModel> snoozeChimeVolumeModel)
            : AbstractSnoozeSettingsModel(std::move(snoozeOnOffModel),
                                          std::move(snoozeLengthModel),
                                          std::move(snoozeChimeIntervalModel),
                                          std::move(snoozeChimeToneModel),
                                          std::move(snoozeChimeVolumeModel))
        {}
    };
} // namespace app::bell_settings
