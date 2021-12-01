// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/InternalModel.hpp>
#include <Temperature.hpp>
#include <time/time_locale.hpp>

namespace gui
{
    class TimeSetListItem;

    class TimeFormatSetListItem;

    class TemperatureUnitListItem;
} // namespace gui

namespace app::bell_settings
{
    class TimeUnitsModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
    {
      public:
        explicit TimeUnitsModel(app::ApplicationCommon *app);

        ~TimeUnitsModel();

        auto clearData() -> void;

        auto saveData() -> void;

        virtual auto loadData() -> void;

        auto createData() -> void;

        auto requestRecords(uint32_t offset, uint32_t limit) -> void override;

        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

        [[nodiscard]] auto getTemperatureUnit() const -> utils::temperature::Temperature::Unit;
        auto setTemperatureUnit(utils::temperature::Temperature::Unit unit) -> void;

      protected:
        app::ApplicationCommon *application{};
        gui::TimeSetListItem *timeSetListItem{};
        gui::TimeFormatSetListItem *timeFmtSetListItem{};
        gui::TemperatureUnitListItem *temperatureUnitListItem{};

        void sendRtcUpdateTimeMessage(time_t newTime);

        void sendTimeFmtUpdateMessage(utils::time::Locale::TimeFormat newFmt);
    };

    class TimeUnitsModelFactoryResetValues : public TimeUnitsModel
    {
      public:
        using TimeUnitsModel::TimeUnitsModel;
        auto loadData() -> void override;

      private:
        static constexpr int factoryResetTime    = 12 * 60 * 60; // 12:00 PM
        static constexpr auto factoryRestTimeFmt = utils::time::Locale::TimeFormat::FormatTime12H;
    };
} // namespace app::bell_settings
