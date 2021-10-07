// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractPrewakeUpSettingsModel.hpp"
#include <models/SettingsModel.hpp>

namespace app::bell_settings
{
    class PrewakeUpChimeDurationModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
    };

    class PrewakeUpChimeToneModel : public gui::SettingsModel<UTF8>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(UTF8 value) override;
        UTF8 getValue() const override;
    };

    class PrewakeUpChimeVolumeModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
    };

    class PrewakeUpLightDurationModel : public gui::SettingsModel<std::uint8_t>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
    };

    class PrewakeUpSettingsModel : public AbstractPrewakeUpSettingsModel
    {
      public:
        PrewakeUpSettingsModel(std::unique_ptr<PrewakeUpChimeDurationModel> chimeDurationModel,
                               std::unique_ptr<PrewakeUpChimeToneModel> chimeToneModel,
                               std::unique_ptr<PrewakeUpChimeVolumeModel> chimeVolumeModel,
                               std::unique_ptr<PrewakeUpLightDurationModel> lightDurationModel)
            : AbstractPrewakeUpSettingsModel(std::move(chimeDurationModel),
                                             std::move(chimeToneModel),
                                             std::move(chimeVolumeModel),
                                             std::move(lightDurationModel))
        {}
    };
} // namespace app::bell_settings
