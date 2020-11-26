// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <vector>

#include <module-apps/Application.hpp>
#include <module-apps/DatabaseModel.hpp>

#include <module-db/Interface/NotesRecord.hpp>

#include <module-gui/gui/widgets/ListItemProvider.hpp>

#include "NotesRepository.hpp"

namespace app::notes
{
    class NotesListItemProvider : public app::DatabaseModel<NotesRecord>, public gui::ListItemProvider
    {
      public:
        explicit NotesListItemProvider(app::Application *app);
    };

    class NotesListModel : public NotesListItemProvider
    {
      public:
        NotesListModel(app::Application *app, std::shared_ptr<AbstractNotesRepository> notesRepository);

        [[nodiscard]] unsigned int requestRecordsCount() override;
        [[nodiscard]] bool updateRecords(std::vector<NotesRecord> records) override;
        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;

        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        [[nodiscard]] unsigned int getMinimalItemHeight() const override;

      private:
        bool onNotesRetrieved(const std::vector<NotesRecord> &records, unsigned int notesRepoCount);

        std::shared_ptr<AbstractNotesRepository> notesRepository;
    };
} // namespace app::notes
