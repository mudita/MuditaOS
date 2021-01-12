// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "QuotesRepository.hpp"

#include <purefs/filesystem_paths.hpp>
#include <module-gui/gui/widgets/ListView.hpp>
#include <module-apps/InternalModel.hpp>

namespace gui
{
    class QuoteWidget;
}

namespace app
{
    class QuotesModel : public app::InternalModel<gui::QuoteWidget *>, public gui::ListItemProvider
    {
      public:
        QuotesModel(app::Application *app, std::unique_ptr<QuotesRepository> repository);

        [[nodiscard]] auto requestRecordsCount() -> unsigned int final;
        [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int final;

        auto getItem(gui::Order order) -> gui::ListItem * final;
        void requestRecords(const uint32_t offset, const uint32_t limit) final;

        void rebuild();

        void remove(const app::QuoteRecord &quote);
        void save(const app::QuoteRecord &quote);

      private:
        void createData();

        app::Application *application                     = nullptr;
        std::unique_ptr<app::QuotesRepository> repository = nullptr;
    };

} // namespace app
