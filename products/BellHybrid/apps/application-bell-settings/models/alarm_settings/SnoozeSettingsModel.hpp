// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractSnoozeSettingsModel.hpp"
#include <common/models/SettingsModel.hpp>
#include <common/models/AudioModel.hpp>
#include <common/SoundsRepository.hpp>

namespace app::bell_settings
{
    class SnoozeOnOffModel : public gui::SettingsModel<bool>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(bool value) -> void override;
        auto getValue() const -> bool override;
    };

    class SnoozeLengthModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(std::uint8_t value) -> void override;
        auto getValue() const -> std::uint8_t override;
    };

    class SnoozeChimeIntervalModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(std::uint8_t value) -> void override;
        auto getValue() const -> std::uint8_t override;
    };

    class SnoozeChimeToneModel : public gui::SettingsModel<UTF8>
    {
      public:
        SnoozeChimeToneModel(sys::Service *app, SimpleSoundsRepository &soundsRepository);

        auto setValue(UTF8 value) -> void override;
        auto getValue() const -> UTF8 override;

      private:
        SimpleSoundsRepository &soundsRepository;
    };

    class SnoozeChimeVolumeModel : public gui::AbstractSettingsModel<std::uint8_t>
    {
      public:
        explicit SnoozeChimeVolumeModel(AbstractAudioModel &audioModel);

        auto setValue(std::uint8_t value) -> void override;
        auto getValue() const -> std::uint8_t override;
        auto restoreDefault() -> void override;

      private:
        AbstractAudioModel &audioModel;
        std::uint8_t defaultValue;
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
