// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractBedtimeModel.hpp"
#include "SettingsModel.hpp"
#include <common/models/AudioModel.hpp>
#include <common/SoundsRepository.hpp>
#include <ApplicationCommon.hpp>

namespace app::bell_bedtime
{
    class BedtimeOnOffModel : public gui::SettingsModel<bool>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(bool value) -> void override;
        auto getValue() const -> bool override;
    };

    class BedtimeTimeModel : public gui::SettingsModel<std::time_t>
    {
      public:
        using SettingsModel::SettingsModel;

        auto setValue(std::time_t value) -> void override;
        auto getValue() const -> std::time_t override;

      private:
        auto writeString(const std::string &str) -> void;
        auto readString() const -> std::string;
        auto getTmFromString(const std::string &str, std::tm &tm) const -> bool;
    };

    class BedtimeVolumeModel : public gui::AbstractSettingsModel<std::uint8_t>
    {
      public:
        explicit BedtimeVolumeModel(AbstractAudioModel &audioModel);

        auto setValue(std::uint8_t value) -> void override;
        auto getValue() const -> std::uint8_t override;
        auto restoreDefault() -> void override;

      private:
        AbstractAudioModel &audioModel;
        std::uint8_t defaultValue;
    };

    class BedtimeToneModel : public gui::SettingsModel<UTF8>
    {
      public:
        BedtimeToneModel(sys::Service *app, SimpleSoundsRepository &soundsRepository);

        auto setValue(UTF8 value) -> void override;
        auto getValue() const -> UTF8 override;

      private:
        SimpleSoundsRepository &soundsRepository;
    };

    class BedtimeModel : public AbstractBedtimeModel
    {
      public:
        BedtimeModel() = delete;

        BedtimeModel(ApplicationCommon *app, AbstractAudioModel &audioModel, SimpleSoundsRepository &soundsRepository)
        {
            bedtimeOnOff  = std::make_unique<bell_bedtime::BedtimeOnOffModel>(app);
            bedtimeTime   = std::make_unique<bell_bedtime::BedtimeTimeModel>(app);
            bedtimeTone   = std::make_unique<bell_bedtime::BedtimeToneModel>(app, soundsRepository);
            bedtimeVolume = std::make_unique<bell_bedtime::BedtimeVolumeModel>(audioModel);
        }

        gui::AbstractSettingsModel<bool> &getBedtimeOnOff() override
        {
            return *bedtimeOnOff;
        }

        gui::AbstractSettingsModel<std::time_t> &getBedtimeTime() override
        {
            return *bedtimeTime;
        }

        gui::AbstractSettingsModel<UTF8> &getBedtimeTone() override
        {
            return *bedtimeTone;
        }

        gui::AbstractSettingsModel<std::uint8_t> &getBedtimeVolume() override
        {
            return *bedtimeVolume;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<bool>> bedtimeOnOff;
        std::unique_ptr<gui::AbstractSettingsModel<std::time_t>> bedtimeTime;
        std::unique_ptr<gui::AbstractSettingsModel<UTF8>> bedtimeTone;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> bedtimeVolume;
    };
} // namespace app::bell_bedtime
