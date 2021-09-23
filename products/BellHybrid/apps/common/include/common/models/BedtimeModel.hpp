// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractBedtimeModel.hpp"
#include "SettingsModel.hpp"
#include <Service/Service.hpp>
#include <ctime>

namespace app::bell_bedtime
{
    inline constexpr auto DEFAULT_BEDTIME_TIME = "21:00";
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

    class BedtimeModel : public AbstractBedtimeModel
    {
      public:
        BedtimeModel() = delete;

        explicit BedtimeModel(sys::Service *app)
        {
            bedtimeOnOff = std::make_unique<bell_bedtime::BedtimeOnOffModel>(app);
            bedtimeTime  = std::make_unique<bell_bedtime::BedtimeTimeModel>(app);
        }
        gui::AbstractSettingsModel<bool> &getBedtimeOnOff() override
        {
            return *bedtimeOnOff;
        }
        gui::AbstractSettingsModel<time_t> &getBedtimeTime() override
        {
            return *bedtimeTime;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<bool>> bedtimeOnOff;
        std::unique_ptr<gui::AbstractSettingsModel<time_t>> bedtimeTime;
    };
} // namespace app::bell_bedtime
