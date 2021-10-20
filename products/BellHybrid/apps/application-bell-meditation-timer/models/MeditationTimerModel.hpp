// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MeditationTimerListItem.hpp"

#include <ListItemProvider.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::meditation
{
    class MeditationTimerModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
    {
      private:
        gui::MeditationTimerListItem *item = nullptr;

      public:
        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;
        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
        void requestRecords(uint32_t offset, uint32_t limit) override;

        [[nodiscard]] std::chrono::minutes getValue() const noexcept;
        void setValue(std::chrono::minutes value);
        auto createData() -> void;
    };
} // namespace app::meditation
