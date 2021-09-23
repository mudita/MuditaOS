// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <models/alarm_settings/AbstractPrewakeUpSettingsModel.hpp>
#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::bell_settings
{
    class PrewakeUpListItemProvider : public app::InternalModel<gui::BellSideListItemWithCallbacks *>,
                                      public gui::ListItemProvider
    {
      public:
        explicit PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &model);

        std::vector<gui::BellSideListItemWithCallbacks *> getListItems();

        auto requestRecords(uint32_t offset, uint32_t limit) -> void override;

        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

        void clearData();

        std::function<void()> onExit;

      private:
        void buildListItems();

        AbstractPrewakeUpSettingsModel &model;
    };

} // namespace app::bell_settings
