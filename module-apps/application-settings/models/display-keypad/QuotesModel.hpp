// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <DatabaseModel.hpp>
#include <service-db/QuotesMessages.hpp>

namespace gui
{
    class QuoteWidget;
}

namespace Quotes
{
    class QuotesModel : public app::DatabaseModel<QuoteRecord>,
                        public gui::ListItemProvider,
                        public app::AsyncCallbackReceiver
    {
      public:
        explicit QuotesModel(app::ApplicationCommon *application);

        [[nodiscard]] auto requestRecordsCount() -> unsigned int final;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int final;

        auto getItem(gui::Order order) -> gui::ListItem * final;
        void requestRecords(const uint32_t offset, const uint32_t limit) final;
        bool updateRecords(std::vector<QuoteRecord> records) override;

        void add(const QuoteRecord &record);
        void edit(const QuoteRecord &record);
        void remove(const QuoteRecord &record);

        auto handleQueryResponse(db::QueryResult *queryResult) -> bool;

      private:
        app::ApplicationCommon *app = nullptr;
    };

} // namespace Quotes
