// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <DatabaseModel.hpp>
#include <service-db/QuotesMessages.hpp>

namespace Quotes
{
    class CategoriesModel : public app::DatabaseModel<CategoryRecord>,
                            public gui::ListItemProvider,
                            public app::AsyncCallbackReceiver
    {
      public:
        explicit CategoriesModel(app::Application *app);
        [[nodiscard]] auto requestRecordsCount() -> unsigned int final;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int final;
        auto getItem(gui::Order order) -> gui::ListItem * final;
        void requestRecords(const uint32_t offset, const uint32_t limit) final;
        bool updateRecords(std::vector<CategoryRecord> records);

      private:
        app::Application *app = nullptr;

        auto handleQueryResponse(db::QueryResult *queryResult) -> bool;
    };
} // namespace Quotes
