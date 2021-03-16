// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/filesystem_paths.hpp>
#include <gui/widgets/ListView.hpp>
#include <InternalModel.hpp>
#include <service-db/QuotesMessages.hpp>

namespace gui
{
    class QuoteWidget;
}

namespace Quotes
{
    class QuotesModel : public app::InternalModel<gui::QuoteWidget *>, public gui::ListItemProvider
    {
      public:
        explicit QuotesModel(app::Application *application);

        [[nodiscard]] auto requestRecordsCount() -> unsigned int final;
        [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int final;

        auto getItem(gui::Order order) -> gui::ListItem * final;
        void requestRecords(const uint32_t offset, const uint32_t limit) final;

        void rebuild();

        void remove(const QuoteRecord &quote);
        void save(const QuoteRecord &quote);

      private:
        void createData();
        auto getCustomCategoryId() -> std::optional<unsigned int>;

        app::Application *app = nullptr;

        static constexpr auto customCategoryName = "Custom";
    };

} // namespace app
