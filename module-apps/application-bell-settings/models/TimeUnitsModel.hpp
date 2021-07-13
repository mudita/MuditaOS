// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TimeSetSpinnerListItem.hpp"

#include <apps-common/Application.hpp>
#include <apps-common/InternalModel.hpp>

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

      private:
        app::Application *application              = nullptr;
        gui::TimeSetSpinnerListItem *timeSetWidget = nullptr;

        void sendRtcUpdateTimeMessage(time_t newTime);
    };
} // namespace app::bell_settings
