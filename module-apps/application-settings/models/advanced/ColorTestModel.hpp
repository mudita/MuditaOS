// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "advanced/ColorTestListItem.hpp"

#include <Application.hpp>
#include <InternalModel.hpp>
#include <ListItemProvider.hpp>
#include <ListView.hpp>

namespace gui
{
    class ColorTestModel : public app::InternalModel<gui::ColorTestListItem *>, public gui::ListItemProvider
    {
      private:
        app::Application *application = nullptr;

      public:
        explicit ColorTestModel(app::Application *app);

        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;
        gui::ListItem *getItem(gui::Order order) override;
        void requestRecords(const uint32_t offset, const uint32_t limit) override;
        void createData();

        [[nodiscard]] gui::ColorScheme getColorScheme();
    };
} // namespace gui
