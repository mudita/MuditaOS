// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractBedtimeModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::bell_bedtime
{
    class BedtimeListItemProvider : public app::InternalModel<gui::BellSideListItemWithCallbacks *>,
                                    public gui::ListItemProvider
    {
      public:
        explicit BedtimeListItemProvider(std::unique_ptr<AbstractBedtimeModel> &&model);
        auto getListItems() -> std::vector<gui::BellSideListItemWithCallbacks *>;
        auto requestRecords(uint32_t offset, uint32_t limit) -> void override;
        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;
        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

        std::function<void()> onExit;

      private:
        void buildListItems();

        std::unique_ptr<AbstractBedtimeModel> model;
    };

} // namespace app::bell_bedtime
