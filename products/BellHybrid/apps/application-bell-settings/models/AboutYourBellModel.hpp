// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Application.hpp>
#include <InternalModel.hpp>
#include <ListItemProvider.hpp>

namespace app::bell_settings
{
    class AboutYourBellModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
    {
      public:
        explicit AboutYourBellModel(app::ApplicationCommon *app);

        void createData();
        void clearData();

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
        auto getItem(gui::Order order) -> gui::ListItem * override;
        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;

      private:
        settings::Settings settings;
    };
} // namespace app::bell_settings
