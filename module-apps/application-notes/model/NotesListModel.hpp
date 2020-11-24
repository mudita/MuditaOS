// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>

#include "Interface/NotesRecord.hpp"
#include "DatabaseModel.hpp"
#include "NotesRecord.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"

namespace app::notes
{
    class NotesProvider : public app::DatabaseModel<NotesRecord>, public gui::ListItemProvider
    {
      public:
        explicit NotesProvider(app::Application *app);
    };

    class NotesListModel : public NotesProvider
    {
      public:
        explicit NotesListModel(app::Application *app);

        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        [[nodiscard]] unsigned int getMinimalItemHeight() const override;

        unsigned int requestRecordsCount() override;
        bool updateRecords(std::vector<NotesRecord> records) override;
        void requestRecords(const uint32_t offset, const uint32_t limit) override;

      private:
        bool handleQueryResponse(db::QueryResult *queryResult);
    };
} // namespace app::notes
