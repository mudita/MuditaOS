// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/TimeSetSpinnerListItem.hpp>
#include <apps-common/Application.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::bell_alarm
{
    class BellAlarmWindowModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
    {
      public:
        explicit BellAlarmWindowModel(app::Application *app);
        ~BellAlarmWindowModel();

        auto clearData() -> void;
        auto saveData() -> void;
        auto loadData() -> void;
        auto createData() -> void;
        auto requestRecords(uint32_t offset, uint32_t limit) -> void;

        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;
        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

      private:
        app::Application *application{nullptr};
        gui::TimeSetSpinnerListItem *timeSetWidget{nullptr};
    };
} // namespace app::bell_settings
