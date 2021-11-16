// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractBedtimeModel.hpp"
#include "SettingsModel.hpp"
#include <Service/Service.hpp>
#include <common/models/AudioModel.hpp>
#include <ApplicationCommon.hpp>
#include <ctime>

namespace app::bell_bedtime
{
    inline constexpr auto DEFAULT_BEDTIME_TIME = "21:00";
    inline constexpr auto DEFAULT_BEDTIME_TONE = "Evening Horizon";

    class BedtimeOnOffModel : public gui::SettingsModel<bool>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(bool value) override;
        auto getValue() const -> bool override;
    };

    class BedtimeTimeModel : public gui::SettingsModel<time_t>
    {
        auto writeString(std::string str) -> void;
        auto readString() const -> std::string;
        auto getTmFromString(const std::string &str, std::tm &tm) const -> bool;

      public:
        using SettingsModel::SettingsModel;

        void setValue(time_t value) override;
        auto getValue() const -> time_t override;
    };

    class BedtimeVolumeModel : public gui::AbstractSettingsModel<std::uint8_t>
    {
      public:
        explicit BedtimeVolumeModel(AbstractAudioModel &audioModel);

        void setValue(std::uint8_t value) override;
        std::uint8_t getValue() const override;
        void restoreDefault() override;

      private:
        AbstractAudioModel &audioModel;
        std::uint8_t defaultValue;
    };

    class AlarmToneModel : public gui::SettingsModel<UTF8>
    {
      public:
        using SettingsModel::SettingsModel;

        void setValue(UTF8) override;
        UTF8 getValue() const override;
    };

    class BedtimeModel : public AbstractBedtimeModel
    {
      public:
        BedtimeModel() = delete;

        explicit BedtimeModel(ApplicationCommon *app, std::unique_ptr<AudioModel> &&audioModel)
        {
            bedtimeOnOff  = std::make_unique<bell_bedtime::BedtimeOnOffModel>(app);
            bedtimeTime   = std::make_unique<bell_bedtime::BedtimeTimeModel>(app);
            bedtimeTone   = std::make_unique<bell_bedtime::AlarmToneModel>(app);
            bedtimeVolume = std::make_unique<bell_bedtime::BedtimeVolumeModel>(*audioModel);
        }
        gui::AbstractSettingsModel<bool> &getBedtimeOnOff() override
        {
            return *bedtimeOnOff;
        }
        gui::AbstractSettingsModel<time_t> &getBedtimeTime() override
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
        std::unique_ptr<AudioModel> audioModel;
        std::unique_ptr<gui::AbstractSettingsModel<bool>> bedtimeOnOff;
        std::unique_ptr<gui::AbstractSettingsModel<time_t>> bedtimeTime;
        std::unique_ptr<gui::AbstractSettingsModel<UTF8>> bedtimeTone;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> bedtimeVolume;
    };
} // namespace app::bell_bedtime
