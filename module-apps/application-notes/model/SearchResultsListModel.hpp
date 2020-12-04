// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <vector>

#include <module-db/Interface/NotesRecord.hpp>

#include <module-apps/InternalModel.hpp>
#include "module-apps/application-notes/widgets/NotesItem.hpp"
#include <module-gui/gui/widgets/ListItemProvider.hpp>

namespace app::notes
{
    class SearchResultsListModel : public InternalModel<gui::NotesItem *>, public gui::ListItemProvider
    {
      public:
        explicit SearchResultsListModel(Application *application);

        void setResults(const std::vector<NotesRecord> &results);

        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;
        unsigned int requestRecordsCount() override;
        unsigned int getMinimalItemHeight() const override;
        gui::ListItem *getItem(gui::Order order) override;

      private:
        gui::NotesItem *createItem(const NotesRecord &record) const;

        Application *application;
    };
} // namespace app::notes
