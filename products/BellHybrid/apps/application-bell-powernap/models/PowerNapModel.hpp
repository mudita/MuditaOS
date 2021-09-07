// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ListItemProvider.hpp>
#include <apps-common/InternalModel.hpp>

namespace gui
{
    class PowerNapListItem;
} // namespace gui

namespace app::powernap
{

    class PowerNapModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
    {
        gui::PowerNapListItem *powerNapItem = nullptr;

        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;
        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
        void requestRecords(uint32_t offset, uint32_t limit) override;

      public:
        [[nodiscard]] std::chrono::minutes getCurrentValue() const noexcept;
        void setValue(std::chrono::minutes napTimeValue);

        auto createData() -> void;
    };
} // namespace app::powernap
