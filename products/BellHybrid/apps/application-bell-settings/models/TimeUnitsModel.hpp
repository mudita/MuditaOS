// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/Application.hpp>
#include <apps-common/InternalModel.hpp>
#include <module-utils/Temperature.hpp>
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
        explicit TimeUnitsModel(app::Application *app);

        ~TimeUnitsModel();

        auto clearData() -> void;

        auto saveData() -> void;

        auto loadData() -> void;

        auto createData() -> void;

        auto requestRecords(uint32_t offset, uint32_t limit) -> void;

        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

        [[nodiscard]] auto getTemperatureUnit() -> utils::temperature::Temperature::Unit;

      private:
        app::Application *application{};
        gui::TimeSetListItem *timeSetListItem{};
        gui::TimeFormatSetListItem *timeFmtSetListItem{};
        gui::TemperatureUnitListItem *temperatureUnitListItem{};

        void sendRtcUpdateTimeMessage(time_t newTime);

        void sendTimeFmtUpdateMessage(utils::time::Locale::TimeFormat newFmt);
    };
} // namespace app::bell_settings
