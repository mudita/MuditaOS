// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::bell_settings
{
    class AbstractFrontlightModel;
    class AbstractBedsideModel;

    class BedsideListItemProvider : public app::InternalModel<gui::BellSideListItemWithCallbacks *>,
                                    public gui::ListItemProvider
    {
      public:
        BedsideListItemProvider(std::unique_ptr<AbstractBedsideModel> &&bedsideModel,
                                std::unique_ptr<AbstractFrontlightModel> &&frontlightModel);

        auto getListItems() -> std::vector<gui::BellSideListItemWithCallbacks *>;
        auto requestRecords(std::uint32_t offset, std::uint32_t limit) -> void override;
        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;
        [[nodiscard]] auto requestRecordsCount() -> unsigned override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned override;
        auto clearData() -> void;

      private:
        auto buildListItems() -> void;

        std::unique_ptr<AbstractBedsideModel> bedsideModel;
        std::unique_ptr<AbstractFrontlightModel> frontlightModel;
    };
} // namespace app::bell_settings
