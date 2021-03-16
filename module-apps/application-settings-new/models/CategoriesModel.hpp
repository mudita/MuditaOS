// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "QuotesRepository.hpp"

#include <module-gui/gui/widgets/ListView.hpp>
#include <module-apps/InternalModel.hpp>

namespace gui
{
    class CategoryWidget;
}

namespace Quotes
{
    class CategoriesModel : public app::InternalModel<gui::CategoryWidget *>, public gui::ListItemProvider
    {
      private:
        app::Application *application = nullptr;

        auto getCategoryList() -> std::optional<std::vector<CategoryRecord>>;

      public:
        explicit CategoriesModel(app::Application *app);
        [[nodiscard]] auto requestRecordsCount() -> unsigned int final;
        [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int final;
        auto getItem(gui::Order order) -> gui::ListItem * final;
        void requestRecords(const uint32_t offset, const uint32_t limit) final;
        void createData();
        void rebuild();
    };
} // namespace Quotes
